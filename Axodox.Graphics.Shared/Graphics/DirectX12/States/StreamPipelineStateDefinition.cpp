#include "pch.h"
#include "StreamPipelineStateDefinition.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  void StreamPipelineStateDefinition::AddRootSignature(const RootSignatureBase& rootSignature)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE);
    WriteAligned(rootSignature.get());
  }

  void StreamPipelineStateDefinition::AddVertexShader(const VertexShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddDomainShader(const DomainShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddHullShader(const HullShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddGeometryShader(const GeometryShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddPixelShader(const PixelShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddComputeShader(const ComputeShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddAmplificationShader(const AmplificationShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddMeshShader(const MeshShader& shader)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS);
    WriteAligned(D3D12_SHADER_BYTECODE(shader));
  }

  void StreamPipelineStateDefinition::AddBlendState(const BlendState& state)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND);
    WriteAligned(D3D12_BLEND_DESC(state));
  }

  void StreamPipelineStateDefinition::AddRasterizerState(const RasterizerState& state)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER);
    WriteAligned(D3D12_RASTERIZER_DESC(state));
  }

  void StreamPipelineStateDefinition::AddDepthStencilState(const DepthStencilState& state)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL);
    WriteAligned(D3D12_DEPTH_STENCIL_DESC(state));
  }

  void StreamPipelineStateDefinition::AddSampleState(const SampleState& state)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC);
    WriteAligned(DXGI_SAMPLE_DESC(state));
  }

  void StreamPipelineStateDefinition::AddSampleMask(uint32_t mask)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK);
    WriteAligned(mask);
  }

  void StreamPipelineStateDefinition::AddInputLayout(VertexDefinition inputLayout)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT);
    WriteAligned(D3D12_INPUT_LAYOUT_DESC{ inputLayout.data(), uint32_t(inputLayout.size()) });
  }

  void StreamPipelineStateDefinition::AddPrimitiveTopology(PrimitiveTopologyType topology)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY);
    WriteAligned(D3D12_PRIMITIVE_TOPOLOGY_TYPE(topology));
  }

  void StreamPipelineStateDefinition::AddRenderTargetFormats(std::initializer_list<Format> formats)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS);

    D3D12_RT_FORMAT_ARRAY value;
    value.NumRenderTargets = uint32_t(min(formats.size(), size(value.RTFormats)));
    memcpy(value.RTFormats, formats.begin(), sizeof(DXGI_FORMAT) * value.NumRenderTargets);
    memset(value.RTFormats + formats.size(), DXGI_FORMAT_UNKNOWN, sizeof(DXGI_FORMAT) * (size(value.RTFormats) - value.NumRenderTargets));

    WriteAligned(value);
  }

  void StreamPipelineStateDefinition::AddDepthStencilFormat(Format format)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT);
    WriteAligned(DXGI_FORMAT(format));
  }

  void StreamPipelineStateDefinition::AddCachedPso(std::span<const uint8_t> buffer)
  {
    WriteAligned(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO);
    WriteAligned(D3D12_CACHED_PIPELINE_STATE{ buffer.data(), buffer.size() });
  }

  StreamPipelineStateDefinition::operator D3D12_PIPELINE_STATE_STREAM_DESC() const
  {
    return D3D12_PIPELINE_STATE_STREAM_DESC{
      .SizeInBytes = _stream.length(),
      .pPipelineStateSubobjectStream = const_cast<uint8_t*>(_stream.data())
    };
  }

  template<typename T>
  void StreamPipelineStateDefinition::WriteAligned(const T& value)
  {
    //Align writes according to DirectX API specification
    auto alignment = is_same_v<T, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE> ? sizeof(void*) : alignof(T);
    while (_stream.position() % alignment != 0)
    {
      _stream.write(0ui8);
    }

    //Write value
    _stream.write(value);
  }
}