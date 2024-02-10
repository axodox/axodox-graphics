#include "pch.h"
#include "GraphicsPipelineStateDefinition.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Infrastructure;
using namespace std;

namespace Axodox::Graphics::D3D12
{
  void GraphicsPipelineStateDefinition::AddCachedPso(std::span<const uint8_t> buffer)
  {
    CachedPso = buffer;
  }

  GraphicsPipelineStateDefinition::operator D3D12_GRAPHICS_PIPELINE_STATE_DESC() const
  {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC result{
      .pRootSignature = RootSignature->get(),
      .VS = VertexShader ? D3D12_SHADER_BYTECODE(*VertexShader) : D3D12_SHADER_BYTECODE{ nullptr, 0 },
      .PS = PixelShader ? D3D12_SHADER_BYTECODE(*PixelShader) : D3D12_SHADER_BYTECODE{ nullptr, 0 },
      .DS = DomainShader ? D3D12_SHADER_BYTECODE(*DomainShader) : D3D12_SHADER_BYTECODE{ nullptr, 0 },
      .HS = HullShader ? D3D12_SHADER_BYTECODE(*HullShader) : D3D12_SHADER_BYTECODE{ nullptr, 0 },
      .GS = GeometryShader ? D3D12_SHADER_BYTECODE(*GeometryShader) : D3D12_SHADER_BYTECODE{ nullptr, 0 },
      .StreamOutput = { nullptr, 0u, nullptr, 0u, 0u },
      .BlendState = D3D12_BLEND_DESC(BlendState),
      .SampleMask = SampleMask,
      .RasterizerState = D3D12_RASTERIZER_DESC(RasterizerState),
      .DepthStencilState = D3D12_DEPTH_STENCIL_DESC(DepthStencilState),
      .InputLayout = { InputLayout.data(), uint32_t(InputLayout.size()) },
      .IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
      .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE(TopologyType),
      .NumRenderTargets = uint32_t(RenderTargetFormats.size()),
      .DSVFormat = DXGI_FORMAT(DepthStencilFormat),
      .SampleDesc = DXGI_SAMPLE_DESC(SampleState),
      .NodeMask = 0u,
      .CachedPSO = { CachedPso.data(), CachedPso.size() },
      .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };

    zero_memory(result.RTVFormats);
    memcpy(result.RTVFormats, RenderTargetFormats.begin(), min(RenderTargetFormats.size(), size(result.RTVFormats)));

    return result;
  }
}