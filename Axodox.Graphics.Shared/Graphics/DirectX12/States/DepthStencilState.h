#pragma once
#include "../Enumerations.h"

namespace Axodox::Graphics::D3D12
{
  enum class DepthStencilMode
  {
    IgnoreDepth,
    ReadDepth,
    WriteDepth
  };

  struct DepthStencilState
  {
    bool WriteDepth;
    ComparisonFunction Comparison;

    DepthStencilState();
    DepthStencilState(DepthStencilMode mode);

    explicit operator D3D12_DEPTH_STENCIL_DESC() const;
  };
}