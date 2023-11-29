#pragma once
#include "../Enumerations.h"

namespace Axodox::Graphics::D3D12
{
  struct DepthStencilState
  {
    bool WriteDepth = true;
    ComparisonFunction Comparison = ComparisonFunction::Less;

    explicit operator D3D12_DEPTH_STENCIL_DESC() const;
  };
}