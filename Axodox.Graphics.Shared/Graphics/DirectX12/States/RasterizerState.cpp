#include "pch.h"
#include "RasterizerState.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Infrastructure;

namespace Axodox::Graphics::D3D12
{
  RasterizerState::operator D3D12_RASTERIZER_DESC() const
  {
    D3D12_RASTERIZER_DESC result;
    zero_memory(result);

    if (has_flag(Flags, RasterizerFlags::CullClockwise))
    {
      result.FrontCounterClockwise = true;
      result.CullMode = D3D12_CULL_MODE_BACK;
    }
    else if (has_flag(Flags, RasterizerFlags::CullCounterClockwise))
    {
      result.CullMode = D3D12_CULL_MODE_BACK;
    }
    else
    {
      result.CullMode = D3D12_CULL_MODE_NONE;
    }

    if (has_flag(Flags, RasterizerFlags::Wireframe))
    {
      result.FillMode = D3D12_FILL_MODE_WIREFRAME;
    }
    else
    {
      result.FillMode = D3D12_FILL_MODE_SOLID;
    }

    if (!has_flag(Flags, RasterizerFlags::DisableDepthClip))
    {
      result.DepthClipEnable = true;
    }

    result.DepthBias = DepthBias;
    result.SlopeScaledDepthBias = SlopeScaledDepthBias;
    result.MultisampleEnable = true;

    return result;
  }
}