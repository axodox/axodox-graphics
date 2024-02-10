#pragma once
#include "Storage/MemoryStream.h"
#include "RootSignature.h"
#include "Shaders.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "SampleState.h"
#include "../Meshes/VertexDefinitions.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API StreamPipelineStateDefinition
  {
  public:
    void AddRootSignature(const RootSignatureBase& rootSignature);

    void AddVertexShader(const VertexShader& shader);
    void AddDomainShader(const DomainShader& shader);
    void AddHullShader(const HullShader& shader);
    void AddGeometryShader(const GeometryShader& shader);
    void AddPixelShader(const PixelShader& shader);
    void AddComputeShader(const ComputeShader& shader);
    void AddAmplificationShader(const AmplificationShader& shader);
    void AddMeshShader(const MeshShader& shader);

    void AddBlendState(const BlendState& state);
    void AddRasterizerState(const RasterizerState& state);
    void AddDepthStencilState(const DepthStencilState& state);
    void AddSampleState(const SampleState& state);
    void AddSampleMask(uint32_t mask);

    void AddInputLayout(VertexDefinition inputLayout);
    void AddPrimitiveTopology(PrimitiveTopologyType topology);

    void AddRenderTargetFormats(std::initializer_list<Format> formats);
    void AddDepthStencilFormat(Format format);

    void AddCachedPso(std::span<const uint8_t> buffer);

    explicit operator D3D12_PIPELINE_STATE_STREAM_DESC() const;
    
  private:
    Storage::memory_stream _stream;

    template<typename T>
    void WriteAligned(const T& value);
  };
}