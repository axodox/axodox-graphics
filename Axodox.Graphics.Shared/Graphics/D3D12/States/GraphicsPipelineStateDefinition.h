#pragma once
#include "RootSignature.h"
#include "Shaders.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "SampleState.h"
#include "../Meshes/VertexDefinitions.h"

namespace Axodox::Graphics::D3D12
{
  struct GraphicsPipelineStateDefinition
  {
    RootSignatureBase* RootSignature = nullptr;

    VertexShader* VertexShader = nullptr;
    DomainShader* DomainShader = nullptr;
    HullShader* HullShader = nullptr;
    GeometryShader* GeometryShader = nullptr;
    PixelShader* PixelShader = nullptr;

    BlendState BlendState = {};
    RasterizerState RasterizerState = {};
    DepthStencilState DepthStencilState = {};
    SampleState SampleState = {};
    uint32_t SampleMask = 0xffffffffu;

    VertexDefinition InputLayout = {};
    PrimitiveTopologyType TopologyType = PrimitiveTopologyType::Triangle;

    std::initializer_list<Format> RenderTargetFormats;
    Format DepthStencilFormat = Format::Unknown;

    std::span<const uint8_t> CachedPso = {};

    void AddCachedPso(std::span<const uint8_t> buffer);
    explicit operator D3D12_GRAPHICS_PIPELINE_STATE_DESC() const;
  };
}