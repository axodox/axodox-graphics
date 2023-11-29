#pragma once
#include "Shaders.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "SampleState.h"
#include "RootSignature.h"
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

    VertexDefinition InputLayout = {};
    PrimitiveTopology InputTopology = PrimitiveTopology::TriangleList;

    std::initializer_list<Format> RenderTargetFormats;
    Format DepthStencilFormat = Format::Unknown;

    explicit operator D3D12_GRAPHICS_PIPELINE_STATE_DESC() const;
  };

  struct ComputePipelineStateDefinition
  {
    RootSignatureBase* RootSignature = nullptr;
    ComputeShader* ComputeShader = nullptr;

    explicit operator D3D12_COMPUTE_PIPELINE_STATE_DESC() const;
  };

  class PipelineState
  {
  public:
    explicit PipelineState(winrt::com_ptr<ID3D12PipelineState>&& pipelineState);

    ID3D12PipelineState* get() const;

  private:
    winrt::com_ptr<ID3D12PipelineState> _pipelineState;
  };

  class PipelineStateProvider
  {
  public:
    explicit PipelineStateProvider(const GraphicsDevice& device, const std::filesystem::path& cacheLocation = L"");

    std::future<PipelineState> CreatePipelineStateAsync(const GraphicsPipelineStateDefinition& definition, winrt::guid id = {});
    std::future<PipelineState> CreatePipelineStateAsync(const ComputePipelineStateDefinition& definition, winrt::guid id = {});

  private:
    GraphicsDevice _device;
    std::filesystem::path _cacheLocation;

    std::filesystem::path GetCachedPath(winrt::guid id) const;

    template<typename T>
    using CreatePipelineFunc = HRESULT(ID3D12Device::*)(const T*, const IID&, void**);

    template<typename StateDescription>
    std::future<PipelineState> CreatePipelineStateAsync(StateDescription& description, winrt::guid id, CreatePipelineFunc<StateDescription> createPipeline);
  };
}