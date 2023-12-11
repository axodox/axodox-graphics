#include "pch.h"
#include "TextureDefinition.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Storage;
using namespace std;
using namespace winrt;

namespace {
  IWICImagingFactory* WicFactory()
  {
    static com_ptr<IWICImagingFactory> wicFactory;

    if (!wicFactory)
    {
      check_hresult(CoCreateInstance(
        CLSID_WICImagingFactory, nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), wicFactory.put_void()));
    }

    return wicFactory.get();
  }
}

namespace Axodox::Graphics::D3D12
{
  TextureHeader::TextureHeader() :
    PixelFormat(Format::Unknown),
    Width(0), 
    Height(0), 
    Depth(0),
    MipCount(1),
    ArraySize(0)
  { }

  TextureHeader::TextureHeader(const D3D12_RESOURCE_DESC& description) :
    PixelFormat(Format(description.Format)),
    Width(uint32_t(description.Width)),
    Height(description.Dimension >= D3D12_RESOURCE_DIMENSION_TEXTURE2D ? description.Height : 0u),
    Depth(description.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? description.DepthOrArraySize : 0u),
    ArraySize(description.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D ? description.DepthOrArraySize : 0u),
    MipCount(description.MipLevels)
  { }

  TextureHeader::TextureHeader(Format format, uint32_t width, uint32_t height, uint32_t arraySize) :
    PixelFormat(format),
    Width(width),
    Height(height),
    Depth(0),
    MipCount(1),
    ArraySize(arraySize)
  { }

  TextureDefinition::TextureDefinition() :
    TextureHeader(),
    SampleCount(1),
    SampleQuality(0),
    Flags(TextureFlags::None)
  { }

  TextureDefinition::TextureDefinition(TextureHeader header) :
    TextureHeader(header),
    SampleCount(1),
    SampleQuality(0),
    Flags(TextureFlags::None)
  { }

  TextureDefinition::TextureDefinition(Format format, uint32_t width, uint32_t height, uint16_t arraySize, TextureFlags flags) :
    TextureHeader(format, width, height, arraySize),
    SampleCount(1),
    SampleQuality(0),
    Flags(flags)
  { }

  TextureDefinition TextureDefinition::MakeSizeCompatible(Format format, TextureFlags flags) const
  {
    TextureDefinition result{ *this };
    result.PixelFormat = format;
    result.Flags = flags;
    return result;
  }

  bool TextureDefinition::AreSizeCompatible(const TextureDefinition& a, const TextureDefinition& b)
  {
    return a.Width == b.Width &&
      a.Height == b.Height &&
      a.Depth == b.Depth &&
      a.ArraySize == b.ArraySize &&
      a.MipCount == b.MipCount &&
      a.SampleCount == b.SampleCount &&
      a.SampleQuality == b.SampleQuality;
  }

  TextureDefinition::TextureDefinition(const D3D12_RESOURCE_DESC& description) :
    TextureHeader(description),
    SampleCount(description.SampleDesc.Count),
    SampleQuality(description.SampleDesc.Quality),
    Flags(TextureFlags(description.Flags))
  { }

  ResourceType TextureDefinition::Type() const
  {
    return ResourceType::Texture;
  }

  TextureDefinition::operator D3D12_RESOURCE_DESC() const
  {
    auto dimension = 1u;
    for (auto size : { Width, Height, Depth })
    {
      if (size > 0) dimension++; else break;
    }

    return D3D12_RESOURCE_DESC{
      .Dimension = D3D12_RESOURCE_DIMENSION(dimension),
      .Alignment = 0ull,
      .Width = Width,
      .Height = dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D ? 1u : Height,
      .DepthOrArraySize = uint16_t(max(1u, dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? Depth : ArraySize)),
      .MipLevels = MipCount,
      .Format = DXGI_FORMAT(PixelFormat),
      .SampleDesc = { SampleCount, SampleQuality },
      .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
      .Flags = D3D12_RESOURCE_FLAGS(Flags)
    };
  }

  TextureData::TextureData() :
    _header({})
  { }

  TextureData::TextureData(Format format, uint32_t width, uint32_t height, uint16_t arraySize) :
    _header(format, width, height, arraySize)
  {
    AllocateBuffer();
  }

  TextureData::TextureData(TextureData&& other)
  {
    *this = move(other);
  }

  TextureData& TextureData::operator=(TextureData&& other)
  {
    Reset();

    swap(_header, other._header);
    swap(_buffer, other._buffer);
    swap(_mipLayouts, other._mipLayouts);

    return *this;
  }

  TextureData::operator bool() const
  {
    return !_buffer.empty();
  }

  void TextureData::UpdateLayout()
  {
    _mipLayouts.clear();
    _mipLayouts.reserve(_header.MipCount);

    uint64_t offset = 0;
    for (auto mip = 0; mip < _header.MipCount; mip++)
    {
      auto rowPitch = BitsPerPixel(_header.PixelFormat) * (_header.Width >> mip);
      if (rowPitch % 8 != 0) rowPitch += 8 - rowPitch % 8;
      rowPitch >>= 3;

      auto depthPitch = rowPitch * max(1u, _header.Height >> mip);

      auto mipPitch = max(1u, _header.Depth > 0 ? _header.Depth >> mip : _header.ArraySize) * depthPitch;

      TextureLayout layout{
        .Offset = offset,
        .RowPitch = rowPitch,
        .DepthPitch = depthPitch,
        .MipPitch = mipPitch
      };
      _mipLayouts.push_back(layout);

      offset += layout.MipPitch;
    }
  }

  TextureData TextureData::FromBuffer(std::span<const uint8_t> buffer)
  {
    if (buffer.empty()) return {};

    auto wicFactory = WicFactory();

    //Load data to WIC bitmap
    com_ptr<IWICBitmapSource> wicBitmap;
    {
      auto stream = to_stream(buffer);

      com_ptr<IWICBitmapDecoder> wicBitmapDecoder;
      check_hresult(wicFactory->CreateDecoderFromStream(
        stream.get(),
        NULL,
        WICDecodeMetadataCacheOnDemand,
        wicBitmapDecoder.put()
      ));

      check_hresult(wicBitmapDecoder->GetFrame(0, reinterpret_cast<IWICBitmapFrameDecode**>(wicBitmap.put())));
    }

    //Covert data to target format
    {
      com_ptr<IWICFormatConverter> wicFormatConverter;
      check_hresult(wicFactory->CreateFormatConverter(wicFormatConverter.put()));
      check_hresult(wicFormatConverter->Initialize(
        wicBitmap.get(), GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone, nullptr, 0.f,
        WICBitmapPaletteTypeMedianCut));
      wicBitmap = move(wicFormatConverter);
    }

    //Define result
    uint32_t width, height, stride;
    check_hresult(wicBitmap->GetSize(&width, &height));

    TextureData result{ Format::B8G8R8A8_UNorm_SRGB, width, height };
    auto span = result.AsRawSpan(&stride);

    WICRect rect = { 0, 0, int32_t(width), int32_t(height) };
    check_hresult(wicBitmap->CopyPixels(&rect, stride, uint32_t(span.size()), span.data()));

    return result;
  }

  TextureData TextureData::FromFile(const std::filesystem::path& path)
  {
    auto buffer = try_read_file(path);
    return FromBuffer(buffer);
  }

  TextureDefinition TextureData::Definition() const
  {
    return Header();
  }

  void TextureData::CopyToResource(ID3D12Resource* resource) const
  {
    com_ptr<ID3D12Device> device;
    check_hresult(resource->GetDevice(IID_PPV_ARGS(device.put())));

    auto description = D3D12_RESOURCE_DESC(Definition());
    vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts(description.DepthOrArraySize * description.MipLevels);
    device->GetCopyableFootprints(&description, 0, uint32_t(layouts.size()), 0ull, layouts.data(), nullptr, nullptr, nullptr);

    uint8_t* pBuffer;
    check_hresult(resource->Map(0u, &EmptyRange, reinterpret_cast<void**>(&pBuffer)));

    for (auto mip = 0u; mip < description.MipLevels; mip++)
    {
      for (auto slice = 0u; slice < description.DepthOrArraySize; slice++)
      {
        auto subresourceIndex = GetSubresourceIndex(mip, slice, 0u, description.MipLevels, description.DepthOrArraySize);
        auto& layout = layouts[subresourceIndex];

        uint32_t sourcePitch, targetPitch = layout.Footprint.RowPitch;
        auto bytes = AsRawSpan(&sourcePitch, slice, mip);

        auto stride = min(sourcePitch, layout.Footprint.RowPitch);
        auto pSource = bytes.data();
        auto pTarget = pBuffer + layout.Offset;
        for (auto row = 0u; row < _header.Height; row++)
        {
          memcpy(pTarget, pSource, stride);
          pTarget += targetPitch;
          pSource += sourcePitch;
        }
      }
    }

    resource->Unmap(0, nullptr);
  }

  const TextureHeader& TextureData::Header() const
  {
    return _header;
  }

  void TextureData::Reset()
  {
    _header = {};
    _buffer.clear();
    _mipLayouts.clear();
  }

  void TextureData::AllocateBuffer()
  {
    //Update layout
    UpdateLayout();

    //Allocate layout
    uint64_t size = 0;
    for (auto& mip : _mipLayouts)
    {
      size += mip.MipPitch;
    }

    _buffer.resize(size);
  }

  std::span<uint8_t> TextureData::AsRawSpan(uint32_t* stride, uint32_t slice, uint32_t mip)
  {
    if (_buffer.empty()) return {};
    if (mip > _header.MipCount) throw out_of_range("Mip index is out of range!");
    if (slice > max(_header.Depth, _header.ArraySize)) throw out_of_range("Slice index is out of range!");

    auto& mipLayout = _mipLayouts[mip];
    if (stride) *stride = uint32_t(mipLayout.RowPitch);

    return { _buffer.data() + mipLayout.Offset + mipLayout.DepthPitch * slice, mipLayout.DepthPitch };
  }

  std::span<const uint8_t> TextureData::AsRawSpan(uint32_t* stride, uint32_t slice, uint32_t mip) const
  {
    return const_cast<TextureData*>(this)->AsRawSpan(stride, slice, mip);
  }
}