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

  struct TextureDefinition
  {
    uint32_t Width = 0, Height = 0, Depth = 0;
    uint32_t SampleCount = 1, SampleQuality = 0;
    uint16_t MipLevels = 1, ArraySize = 0;
    Format PixelFormat = Format::Unknown;
    TextureFlags Flags = TextureFlags::None;

    explicit operator D3D12_RESOURCE_DESC() const;
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