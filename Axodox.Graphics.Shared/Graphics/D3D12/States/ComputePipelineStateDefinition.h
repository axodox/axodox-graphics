#pragma once
#include "RootSignature.h"
#include "Shaders.h"

namespace Axodox::Graphics::D3D12
{
  struct AXODOX_GRAPHICS_API ComputePipelineStateDefinition
  {
    RootSignatureBase* RootSignature = nullptr;
    ComputeShader* ComputeShader = nullptr;
    std::span<const uint8_t> CachedPso = {};

    void AddCachedPso(std::span<const uint8_t> buffer);
    explicit operator D3D12_COMPUTE_PIPELINE_STATE_DESC() const;
  };
}