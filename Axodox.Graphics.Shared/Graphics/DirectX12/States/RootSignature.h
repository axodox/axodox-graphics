#pragma once
#include "../Devices/GraphicsDevice.h"
#include "RootParameters.h"
#include "StaticSampler.h"

namespace Axodox::Graphics::D3D12
{
  struct RootParameter;

  enum class RootSignatureFlags
  {
    None = D3D12_ROOT_SIGNATURE_FLAG_NONE,
    AllowInputAssemblerInputLayout = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT,
    DenyVertexShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS,
    DenyHullShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS,
    DenyDomainShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS,
    DenyGeometryShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS,
    DenyPixelShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS,
    AllowStreamOutput = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT,
    LocalRootSignature = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE,
    DenyAmplificationShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS,
    DenyMeshShaderRootAccess = D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS,
    CbvSrvUavHeapDirectlyIndexed = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
    SamplerHeapDirectlyIndexed = D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED
  };

  class RootSignatureBase
  {
    friend struct RootParameter;
    friend struct StaticSampler;

  public:
    RootSignatureBase(const GraphicsDevice& device);

    RootSignatureFlags Flags = RootSignatureFlags::None;

    ID3D12RootSignature* get();

    void SetGraphics(CommandAllocator& allocator);
    void SetCompute(CommandAllocator& allocator);

  private:
    GraphicsDevice _device;
    std::vector<ptrdiff_t> _parameterOffsets;
    std::vector<ptrdiff_t> _samplerOffsets;
    winrt::com_ptr<ID3D12RootSignature> _rootSignature;

    void AddParameter(RootParameter* parameter);
    void AddSampler(const StaticSampler* sampler);
    winrt::com_ptr<ID3D12RootSignature> Build() const;
  };
}