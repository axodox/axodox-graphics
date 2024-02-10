#include "pch.h"
#include "BlendState.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Graphics::D3D12;
using namespace Axodox::Infrastructure;
using namespace std;

namespace {
  D3D12_RENDER_TARGET_BLEND_DESC GetRenderTargetBlendDescription(BlendType type)
  {
    D3D12_RENDER_TARGET_BLEND_DESC result;
    zero_memory(result);

    switch (type)
    {
    case BlendType::Opaque:
      result.BlendEnable = false;
      result.SrcBlend = D3D12_BLEND_ONE;
      result.DestBlend = D3D12_BLEND_ZERO;
      result.BlendOp = D3D12_BLEND_OP_ADD;
      result.SrcBlendAlpha = D3D12_BLEND_ONE;
      result.DestBlendAlpha = D3D12_BLEND_ZERO;
      result.BlendOpAlpha = D3D12_BLEND_OP_ADD;
      break;
    case BlendType::Additive:
    case BlendType::Subtractive:
      result.BlendEnable = true;
      result.SrcBlend = D3D12_BLEND_ONE;
      result.DestBlend = D3D12_BLEND_ONE;
      result.BlendOp = result.BlendOpAlpha = (type == Additive ? D3D12_BLEND_OP_ADD : D3D12_BLEND_OP_REV_SUBTRACT);
      result.SrcBlendAlpha = D3D12_BLEND_ONE;
      result.DestBlendAlpha = D3D12_BLEND_ONE;
      result.BlendOpAlpha = result.BlendOp;
      break;
    case BlendType::AlphaBlend:
      result.BlendEnable = true;
      result.SrcBlend = D3D12_BLEND_SRC_ALPHA;
      result.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
      result.BlendOp = D3D12_BLEND_OP_ADD;
      result.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
      result.DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
      result.BlendOpAlpha = D3D12_BLEND_OP_ADD;
      break;
    case BlendType::Minimum:
      result.BlendEnable = true;
      result.SrcBlend = D3D12_BLEND_ONE;
      result.DestBlend = D3D12_BLEND_ONE;
      result.BlendOp = D3D12_BLEND_OP_MIN;
      result.SrcBlendAlpha = D3D12_BLEND_ONE;
      result.DestBlendAlpha = D3D12_BLEND_ONE;
      result.BlendOpAlpha = D3D12_BLEND_OP_MIN;
      break;
    case BlendType::Maximum:
      result.BlendEnable = true;
      result.SrcBlend = D3D12_BLEND_ONE;
      result.DestBlend = D3D12_BLEND_ONE;
      result.BlendOp = D3D12_BLEND_OP_MAX;
      result.SrcBlendAlpha = D3D12_BLEND_ONE;
      result.DestBlendAlpha = D3D12_BLEND_ONE;
      result.BlendOpAlpha = D3D12_BLEND_OP_MAX;
      break;
    }

    result.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    return result;
  }
}

namespace Axodox::Graphics::D3D12
{
  BlendState::BlendState(BlendType blendType)
  {
    zero_memory(BlendTypes);
    BlendTypes[0] = blendType;
  }

  BlendState::BlendState(std::initializer_list<BlendType> blendTypes)
  {
    zero_memory(BlendTypes);
    memcpy(BlendTypes.data(), blendTypes.begin(), min(BlendTypes.size(), blendTypes.size()));
  }

  BlendState::operator D3D12_BLEND_DESC() const
  {
    D3D12_BLEND_DESC result;
    zero_memory(result);

    result.IndependentBlendEnable = BlendTypes.size() > 1;

    for (auto i = 0; i < BlendTypes.size(); i++)
    {
      result.RenderTarget[i] = GetRenderTargetBlendDescription(BlendTypes[i]);
    }

    return result;
  }
}