#pragma once
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::D3D12
{
  enum class TextureFlags
  {
    None = D3D12_RESOURCE_FLAG_NONE,
    RenderTarget = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
    DepthStencil = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
    UnorderedAccess = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
    SimultaneousAccess = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
  };

  struct TextureHeader
  {
    Format PixelFormat;
    uint32_t Width, Height, Depth, ArraySize;
    uint16_t MipCount;

    TextureHeader();
    TextureHeader(Format format, uint32_t width, uint32_t height, uint32_t arraySize);
  };

  struct TextureDefinition : public TextureHeader
  {
    uint32_t SampleCount, SampleQuality;
    TextureFlags Flags;

    TextureDefinition();
    TextureDefinition(Format format, uint32_t width, uint32_t height, uint16_t arraySize = 0, TextureFlags flags = TextureFlags::None);

    explicit operator D3D12_RESOURCE_DESC() const;
  };

  struct TextureLayout
  {
    uint64_t Offset;
    uint64_t RowPitch;
    uint64_t DepthPitch;
    uint64_t MipPitch;
  };

  struct TextureData
  {
    TextureData();
    TextureData(Format format, uint32_t width, uint32_t height, uint16_t arraySize = 0);

    TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;

    TextureData(TextureData&& other);
    TextureData& operator=(TextureData&& other);

    const TextureHeader& Header() const;

    explicit operator bool() const;

    std::span<uint8_t> AsRawSpan(uint64_t* stride = nullptr, uint32_t slice = 0, uint32_t mip = 0);

    template<typename T>
    std::span<T> AsTypedSpan(uint64_t* stride = nullptr, uint32_t slice = 0, uint32_t mip = 0)
    {
      auto rawSpan = AsRawSpan(stride, slice, mip);
      return { reinterpret_cast<T*>(rawSpan.data()), rawSpan.size() / sizeof(T) };
    }

    void Reset();

  private: 
    TextureHeader _header;
    std::vector<uint8_t, Collections::aligned_allocator<uint8_t>> _buffer;
    std::vector<TextureLayout> _mipLayouts;

    void AllocateBuffer();
    void UpdateLayout();
  };

  enum class BufferFlags
  {
    None = D3D12_RESOURCE_FLAG_NONE,
    UnorderedAccess = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
    SimultaneousAccess = D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
  };

  struct BufferDefinition
  {
    uint64_t Length = 0;
    BufferFlags Flags = BufferFlags::None;

    explicit operator D3D12_RESOURCE_DESC() const;
  };
}