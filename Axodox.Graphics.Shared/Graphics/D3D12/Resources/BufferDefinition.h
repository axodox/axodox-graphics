#pragma once
#include "ResourceData.h"
#include "ResourceDefinition.h"
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::D3D12
{
  enum class BufferFlags
  {
    None = D3D12_RESOURCE_FLAG_NONE,
    UnorderedAccess = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
    SimultaneousAccess = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
  };

  class BufferData;

  struct AXODOX_GRAPHICS_API BufferDefinition : public ResourceDefinition
  {
    uint64_t Length = 0;
    BufferFlags Flags = BufferFlags::None;

    BufferDefinition();

    explicit BufferDefinition(uint64_t length, BufferFlags flags = BufferFlags::None);
    explicit BufferDefinition(const BufferData& data, BufferFlags flags = BufferFlags::None);
    BufferDefinition(const D3D12_RESOURCE_DESC& description);

    template<typename T>
    explicit BufferDefinition(std::span<const T> span, BufferFlags flags = BufferFlags::None) :
      Length(sizeof(T)* span.size()),
      Flags(flags)
    { }
        
    template<typename T>
    static BufferDefinition Create(size_t size, BufferFlags flags = BufferFlags::None)
    {
      return { sizeof(T) * size, flags };
    }

    virtual ResourceType Type() const override;
    virtual explicit operator D3D12_RESOURCE_DESC() const override;
  };

  class AXODOX_GRAPHICS_API BufferData : public ResourceData
  {
  public:
    BufferData();
    BufferData(size_t itemSize, size_t itemCount);

    BufferData(const BufferData&) = default;
    BufferData& operator=(const BufferData&) = default;

    BufferData(BufferData&& other);
    BufferData& operator=(BufferData&& other);

    template<typename T>
    BufferData(size_t itemCount, T*& pStart) :
      _itemSize(sizeof(T)),
      _buffer(_itemSize * itemCount)
    {
      pStart = reinterpret_cast<T*>(_buffer.data());
    }
    
    template<typename T>
    BufferData(std::span<const T> span) :
      _itemSize(sizeof(T)),
      _buffer(Infrastructure::to_vector(span))
    { }

    template<typename T>
    BufferData(std::initializer_list<T> list) :
      _itemSize(sizeof(T)),
      _buffer(Infrastructure::to_vector(list))
    { }

    template<typename T>
    static BufferData Create(size_t size, T** pStart = nullptr)
    {
      BufferData result(sizeof(T), size);
      if (pStart) pStart = reinterpret_cast<T*>(result._buffer.data());
      return result;
    }

    explicit operator bool() const;

    std::span<uint8_t> AsRawSpan();

    std::span<const uint8_t> AsRawSpan() const;

    template<typename T>
    std::span<T> AsTypedSpan()
    {
      assert(sizeof(T) == _itemSize);
      return { reinterpret_cast<T*>(_buffer.data()), _buffer.size() / _itemSize };
    }

    virtual void CopyToResource(ID3D12Resource* resource) const override;

    void Reset();

    uint32_t ByteCount() const;
    uint32_t ItemCount() const;
    uint32_t ItemSize() const;

  private:
    size_t _itemSize;
    std::vector<uint8_t> _buffer;    
  };
}