#include "pch.h"
#include "DepthStencilState.h"

namespace Axodox::Graphics::D3D12
{
  DepthStencilState::operator D3D12_DEPTH_STENCIL_DESC() const
  {
    D3D12_DEPTH_STENCIL_DESC result{
      .DepthEnable = Comparison != ComparisonFunction::Always,
      .DepthWriteMask = WriteDepth ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO,
      .DepthFunc = D3D12_COMPARISON_FUNC(Comparison),
      .StencilEnable = false,
      .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
      .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
      .FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS },
      .BackFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS }
    };

    return result;
  }
}