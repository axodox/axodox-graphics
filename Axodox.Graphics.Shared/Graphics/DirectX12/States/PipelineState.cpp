#include "pch.h"
#include "PipelineState.h"
#include "Threading/ThreadPool.h"

using namespace Axodox::Infrastructure;
using namespace Axodox::Storage;
using namespace Axodox::Threading;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
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
      .SampleMask = 0xffffffffu,
      .RasterizerState = D3D12_RASTERIZER_DESC(RasterizerState),
      .DepthStencilState = D3D12_DEPTH_STENCIL_DESC(DepthStencilState),
      .InputLayout = { InputLayout.data(), uint32_t(InputLayout.size()) },
      .IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
      .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE(TopologyType),
      .NumRenderTargets = uint32_t(RenderTargetFormats.size()),
      .DSVFormat = DXGI_FORMAT(DepthStencilFormat),
      .SampleDesc = DXGI_SAMPLE_DESC(SampleState),
      .NodeMask = 0u,
      .CachedPSO = { nullptr, 0u },
      .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };

    zero_memory(result.RTVFormats);
    memcpy(result.RTVFormats, RenderTargetFormats.begin(), min(RenderTargetFormats.size(), size(result.RTVFormats)));

    return result;
  }

  ComputePipelineStateDefinition::operator D3D12_COMPUTE_PIPELINE_STATE_DESC() const
  {
    D3D12_COMPUTE_PIPELINE_STATE_DESC result{
      .pRootSignature = RootSignature->get(),
      .CS = D3D12_SHADER_BYTECODE(*ComputeShader),
      .NodeMask = 0u,
      .CachedPSO = { nullptr, 0u },
      .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };

    return result;
  }

  PipelineState::PipelineState(winrt::com_ptr<ID3D12PipelineState>&& pipelineState) :
    _pipelineState(move(pipelineState))
  { }

  ID3D12PipelineState* PipelineState::get() const
  {
    return _pipelineState.get();
  }

  PipelineStateProvider::PipelineStateProvider(const GraphicsDevice& device, const std::filesystem::path& cacheLocation) :
    _device(device),
    _cacheLocation(cacheLocation)
  { }

  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(const GraphicsPipelineStateDefinition& definition, winrt::guid id)
  {
    auto description = D3D12_GRAPHICS_PIPELINE_STATE_DESC(definition);
    return CreatePipelineStateAsync(description, id, &ID3D12Device::CreateGraphicsPipelineState);
  }

  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(const ComputePipelineStateDefinition& definition, winrt::guid id)
  {
    auto description = D3D12_COMPUTE_PIPELINE_STATE_DESC(definition);
    return CreatePipelineStateAsync(description, id, &ID3D12Device::CreateComputePipelineState);
  }

  template<typename StateDescription>
  std::future<PipelineState> PipelineStateProvider::CreatePipelineStateAsync(StateDescription& description, winrt::guid id, CreatePipelineFunc<StateDescription> createPipeline)
  {
    //Try load cached PSO
    auto cachedPath = GetCachedPath(id);

    vector<uint8_t> cachedBuffer;
    if (!cachedPath.empty())
    {
      cachedBuffer = try_read_file(cachedPath);
      description.CachedPSO = {
        .pCachedBlob = cachedBuffer.data(),
        .CachedBlobSizeInBytes = cachedBuffer.size()
      };
    }

    //Run PSO generation on thread-pool
    return threadpool_execute<PipelineState>([=] {
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
