#include "pch.h"
#include "TextureDefinition.h"

using namespace std;

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

  TextureDefinition::TextureDefinition(Format format, uint32_t width, uint32_t height, uint16_t arraySize, TextureFlags flags) :
    TextureHeader(format, width, height, arraySize),
    SampleCount(1),
    SampleQuality(0),
    Flags(flags)
  { }

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
      .DepthOrArraySize = uint16_t(dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? Depth : ArraySize),
      .MipLevels = MipCount,
      .Format = DXGI_FORMAT(PixelFormat),
      .SampleDesc = { SampleCount, SampleQuality },
      .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
      .Flags = D3D12_RESOURCE_FLAGS(Flags)
    };
  }

  BufferDefinition::operator D3D12_RESOURCE_DESC() const
  {
    return D3D12_RESOURCE_DESC{
      .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
      .Alignment = 0ull,
      .Width = Length,
      .Height = 1u,
      .DepthOrArraySize = 1u,
      .MipLevels = 1u,
      .Format = DXGI_FORMAT_UNKNOWN,
      .SampleDesc = { 1u, 0u },
      .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
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
      auto rowPitch = BitsPerPixel(_header.PixelFormat) * _header.Width >> mip;
      if (rowPitch % 8 != 0) rowPitch += 8 - rowPitch % 8;

      auto depthPitch = rowPitch * max(1u, _header.Height >> mip);

      auto mipPitch = max(1u, _header.Depth > 0 ? _header.Depth >> mip : _header.ArraySize);

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

  std::span<uint8_t> TextureData::AsRawSpan(uint64_t* stride, uint32_t slice, uint32_t mip)
  {
    if (_buffer.empty()) return {};
    if (mip > _header.MipCount) throw out_of_range("Mip index is out of range!");
    if (slice > max(_header.Depth, _header.ArraySize)) throw out_of_range("Slice index is out of range!");

    auto& mipLayout = _mipLayouts[mip];
    if (stride) *stride = mipLayout.RowPitch;

    return { _buffer.data() + mipLayout.Offset + mipLayout.DepthPitch * slice, mipLayout.DepthPitch };
  }
}