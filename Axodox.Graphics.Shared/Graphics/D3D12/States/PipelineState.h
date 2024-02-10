#pragma once
#include "GraphicsPipelineStateDefinition.h"
#include "ComputePipelineStateDefinition.h"
#include "StreamPipelineStateDefinition.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API PipelineState
  {
  public:
    explicit PipelineState(winrt::com_ptr<ID3D12PipelineState>&& pipelineState);

    ID3D12PipelineState* get() const;

    void Apply(CommandAllocator& allocator);

  private:
    winrt::com_ptr<ID3D12PipelineState> _pipelineState;
  };

  class AXODOX_GRAPHICS_API PipelineStateProvider
  {
  public:
    explicit PipelineStateProvider(const GraphicsDevice& device, const std::filesystem::path& cacheLocation = L"");

    std::future<PipelineState> CreatePipelineStateAsync(const GraphicsPipelineStateDefinition& definition, winrt::guid id = {});
    std::future<PipelineState> CreatePipelineStateAsync(const ComputePipelineStateDefinition& definition, winrt::guid id = {});
    std::future<PipelineState> CreatePipelineStateAsync(const StreamPipelineStateDefinition& definition, winrt::guid id = {});    

  private:
    GraphicsDevice _device;
    std::filesystem::path _cacheLocation;

    std::filesystem::path GetCachedPath(winrt::guid id) const;

    template<typename T>
    using CreatePipelineFunc = HRESULT(ID3D12DeviceT::*)(const T*, const IID&, void**);

    template<typename StateDescription, typename StateDefinition>
    std::future<PipelineState> CreatePipelineStateAsync(StateDefinition definition, winrt::guid id, CreatePipelineFunc<StateDescription> createPipeline);
  };
}