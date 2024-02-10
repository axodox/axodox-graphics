#include "pch.h"
#include "BufferDefinition.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  BufferDefinition::BufferDefinition() :
    Length(0),
    Flags(BufferFlags::None)
  { }

  BufferDefinition::BufferDefinition(uint64_t length, BufferFlags flags) :
    Length(length),
    Flags(flags)
  { }

  BufferDefinition::BufferDefinition(const BufferData& data, BufferFlags flags) :
    Length(data.AsRawSpan().size()),
    Flags(flags)
  { }

  BufferDefinition::BufferDefinition(const D3D12_RESOURCE_DESC& description) :
    Length(description.Width),
    Flags(BufferFlags(description.Flags))
  { }

  ResourceType BufferDefinition::Type() const
  {
    return ResourceType::Buffer;
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

  BufferData::BufferData() :
    _itemSize(0)
  { }

  BufferData::BufferData(size_t itemSize, size_t itemCount) :
    _itemSize(itemSize),
    _buffer(itemSize* itemCount)
  { }

  BufferData::BufferData(BufferData&& other)
  {
    *this = move(other);
  }

  BufferData& BufferData::operator=(BufferData&& other)
  {
    Reset();
    swap(_itemSize, other._itemSize);
    swap(_buffer, other._buffer);
    return *this;
  }

  BufferData::operator bool() const
  {
    return _itemSize > 0u;
  }

  std::span<uint8_t> BufferData::AsRawSpan()
  {
    return _buffer;
  }

  std::span<const uint8_t> BufferData::AsRawSpan() const
  {
    return _buffer;
  }

  void BufferData::CopyToResource(ID3D12Resource* resource) const
  {
    void* buffer;
    check_hresult(resource->Map(0, &EmptyRange, &buffer));
    memcpy(buffer, _buffer.data(), _buffer.size());
    resource->Unmap(0, nullptr);
  }

  void BufferData::Reset()
  {
    _itemSize = 0;
    _buffer.clear();
  }

  uint32_t BufferData::ByteCount() const
  {
    return uint32_t(_buffer.size());
  }

  uint32_t BufferData::ItemCount() const
  {
    return _itemSize > 0u ? uint32_t(_buffer.size() / _itemSize) : 0u;
  }

  uint32_t BufferData::ItemSize() const
  {
    return uint32_t(_itemSize);
  }
}
