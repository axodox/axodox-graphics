#pragma once
#include "ResourceData.h"
#include "ResourceDefinition.h"
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

  struct AXODOX_GRAPHICS_API TextureHeader
  {
    Format PixelFormat;
    uint32_t Width, Height, Depth, ArraySize;
    uint16_t MipCount;

    TextureHeader();
    TextureHeader(Format format, uint32_t width, uint32_t height, uint32_t arraySize);
    TextureHeader(const D3D12_RESOURCE_DESC& description);
  };

  struct AXODOX_GRAPHICS_API TextureDefinition : public ResourceDefinition, public TextureHeader
  {
    uint32_t SampleCount, SampleQuality;
    TextureFlags Flags;

    TextureDefinition();
    TextureDefinition(TextureHeader header);
    TextureDefinition(Format format, uint32_t width, uint32_t height, uint16_t arraySize = 0, TextureFlags flags = TextureFlags::None);
    TextureDefinition(const D3D12_RESOURCE_DESC& description);

    TextureDefinition MakeSizeCompatible(Format format, TextureFlags flags) const;

    static bool AreSizeCompatible(const TextureDefinition& a, const TextureDefinition& b);

    virtual ResourceType Type() const override;
    virtual explicit operator D3D12_RESOURCE_DESC() const override;
  };

  class TextureData : public ResourceData
  {
    struct TextureLayout
    {
      uint64_t Offset;
      uint64_t RowPitch;
      uint64_t DepthPitch;
      uint64_t MipPitch;
    };

  public:
    TextureData();
    TextureData(Format format, uint32_t width, uint32_t height, uint16_t arraySize = 0);

    TextureData(const TextureData&) = default;
    TextureData& operator=(const TextureData&) = default;

    TextureData(TextureData&& other);
    TextureData& operator=(TextureData&& other);

    const TextureHeader& Header() const;

    explicit operator bool() const;

    std::span<uint8_t> AsRawSpan(uint32_t* stride = nullptr, uint32_t slice = 0, uint32_t mip = 0);
    std::span<const uint8_t> AsRawSpan(uint32_t* stride = nullptr, uint32_t slice = 0, uint32_t mip = 0) const;

    template<typename T>
    std::span<T> AsTypedSpan(uint32_t* stride = nullptr, uint32_t slice = 0, uint32_t mip = 0)
    {
      auto rawSpan = AsRawSpan(stride, slice, mip);
      return { reinterpret_cast<T*>(rawSpan.data()), rawSpan.size() / sizeof(T) };
    }

    virtual void CopyToResource(ID3D12Resource* resource) const override;

    void Reset();

    TextureDefinition Definition() const;

    static TextureData FromBuffer(std::span<const uint8_t> buffer);
    static TextureData FromFile(const std::filesystem::path& path);

  private:
    TextureHeader _header;
    std::vector<uint8_t, Collections::aligned_allocator<uint8_t>> _buffer;
    std::vector<TextureLayout> _mipLayouts;

    void AllocateBuffer();
    void UpdateLayout();
  };
}