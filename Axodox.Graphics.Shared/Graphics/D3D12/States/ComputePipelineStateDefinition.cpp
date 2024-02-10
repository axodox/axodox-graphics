#include "pch.h"
#include "ComputePipelineStateDefinition.h"

namespace Axodox::Graphics::D3D12
{
  void ComputePipelineStateDefinition::AddCachedPso(std::span<const uint8_t> buffer)
  {
    CachedPso = buffer;
  }

  ComputePipelineStateDefinition::operator D3D12_COMPUTE_PIPELINE_STATE_DESC() const
  {
    D3D12_COMPUTE_PIPELINE_STATE_DESC result{
      .pRootSignature = RootSignature->get(),
      .CS = D3D12_SHADER_BYTECODE(*ComputeShader),
      .NodeMask = 0u,
      .CachedPSO = { CachedPso.data(), CachedPso.size() },
      .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };

    return result;
  }
}