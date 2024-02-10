#pragma once

namespace Axodox::Graphics::D3D12
{
  enum class RasterizerFlags
  {
    Default = 0,
    CullNone = 0,
    CullClockwise = 1,
    CullCounterClockwise = 2,
    Wireframe = 4,
    DisableDepthClip = 8
  };

  struct AXODOX_GRAPHICS_API RasterizerState
  {
    RasterizerFlags Flags = {};
    int32_t DepthBias = 0;
    float SlopeScaledDepthBias = 0.f;

    explicit operator D3D12_RASTERIZER_DESC() const;
  };
}