#pragma once
#include "../Devices/GraphicsDevice.h"
#include "../Commands/CommandAllocator.h"

namespace Axodox::Graphics::D3D12
{
  class RootParameter;
  struct StaticSampler;

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

  enum class RootSignatureUsage
  {
    None,
    Graphics,
    Compute
  };

  struct AXODOX_GRAPHICS_API RootSignatureBlueprint
  {
    std::vector<RootParameter*> Parameters;
    std::vector<StaticSampler*> Samplers;
  };

  struct AXODOX_GRAPHICS_API RootSignatureContext
  {
    RootSignatureBlueprint* Blueprint = nullptr;
    CommandAllocator* Allocator = nullptr;
    RootSignatureUsage Usage = {};
  };

  class AXODOX_GRAPHICS_API RootSignatureMask
  {
    friend class RootParameter;
    friend struct StaticSampler;
    friend class RootSignatureBase;

  public:
    RootSignatureMask(const RootSignatureContext& context);

    RootSignatureFlags Flags = RootSignatureFlags::None;

  protected:
    RootSignatureContext _context;

  private:
    void AddParameter(RootParameter* parameter);
    void AddSampler(StaticSampler* sampler);
  };
}