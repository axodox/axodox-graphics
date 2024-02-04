#include "pch.h"
#include "PipelineState.h"
#include "Threading/ThreadPool.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Infrastructure;
using namespace Axodox::Storage;
using namespace Axodox::Threading;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  PipelineState::PipelineState(winrt::com_ptr<ID3D12PipelineState>&& pipelineState) :
    _pipelineState(move(pipelineState))
  { }

  ID3D12PipelineState* PipelineState::get() const
  {
    return _pipelineState.get();
  }

  void PipelineState::Apply(CommandAllocator& allocator)
  {
    allocator->SetPipelineState(_pipelineState.get());
  }

  PipelineStateProvider::PipelineStateProvider(const GraphicsDevice& device, const std::filesystem::path& cacheLocation) :
    _device(device),
    _cacheLocation(cacheLocation)
  { }

  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(const GraphicsPipelineStateDefinition& definition, winrt::guid id)
  {
    return CreatePipelineStateAsync<D3D12_GRAPHICS_PIPELINE_STATE_DESC>(definition, id, &ID3D12DeviceT::CreateGraphicsPipelineState);
  }

  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(const ComputePipelineStateDefinition& definition, winrt::guid id)
  {
    return CreatePipelineStateAsync<D3D12_COMPUTE_PIPELINE_STATE_DESC>(definition, id, &ID3D12DeviceT::CreateComputePipelineState);
  }

  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(const StreamPipelineStateDefinition& definition, winrt::guid id)
  {
    return CreatePipelineStateAsync<D3D12_PIPELINE_STATE_STREAM_DESC>(definition, id, &ID3D12DeviceT::CreatePipelineState);
  }

  template<typename StateDescription, typename StateDefinition>
  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(StateDefinition definition, winrt::guid id, CreatePipelineFunc<StateDescription> createPipeline)
  {
    //Try load cached PSO
    auto cachedPath = GetCachedPath(id);

    vector<uint8_t> cachedBuffer;
    if (!cachedPath.empty())
    {
      cachedBuffer = try_read_file(cachedPath);
      definition.AddCachedPso(cachedBuffer);
    }

    //Run PSO generation on thread-pool
    return threadpool_execute<PipelineState>([=] {
      //Create description
      StateDescription description(definition);

      //Create pipeline state
      com_ptr<ID3D12PipelineState> pipelineState;
      check_hresult((_device.get()->*createPipeline)(&description, IID_PPV_ARGS(pipelineState.put())));

      //Save output to cache
      if (pipelineState && !cachedPath.empty())
      {
        com_ptr<ID3DBlob> cachedBlob;
        check_hresult(pipelineState->GetCachedBlob(cachedBlob.put()));

        span<uint8_t> buffer = { reinterpret_cast<uint8_t*>(cachedBlob->GetBufferPointer()), cachedBlob->GetBufferSize() };
        try_write_file(cachedPath, buffer);
      }

      //Return result
      return PipelineState(move(pipelineState));
      });
  }

  std::filesystem::path PipelineStateProvider::GetCachedPath(winrt::guid id) const
  {
    if (_cacheLocation.empty() || is_default(id)) return L"";

    return _cacheLocation / (to_hstring(id) + L".pso").c_str();
  }
}
