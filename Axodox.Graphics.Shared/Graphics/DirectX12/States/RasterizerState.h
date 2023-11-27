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

  class RasterizerState
  {
    RasterizerFlags Flags = {};
    float DepthBias = 0.f;
    float SlopeScaledDepthBias = 0.f;

    explicit operator D3D12_RASTERIZER_DESC() const;
  };
}