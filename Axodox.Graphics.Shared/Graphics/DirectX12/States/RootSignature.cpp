#include "pch.h"
#include "RootSignature.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  RootSignatureBase::RootSignatureBase(const GraphicsDevice& device) :
    _device(device)
  { }

  ID3D12RootSignature* RootSignatureBase::get()
  {
    if (!_rootSignature)
    {
      _rootSignature = Build();
    }

    return _rootSignature.get();
  }

  void RootSignatureBase::SetGraphics(CommandAllocator& allocator)
  {
    allocator->SetGraphicsRootSignature(get());
  }

  void RootSignatureBase::SetCompute(CommandAllocator& allocator)
  {
    allocator->SetComputeRootSignature(get());
  }

  void RootSignatureBase::AddParameter(RootParameter* parameter)
  {
    parameter->Index = uint32_t(_parameterOffsets.size());
    _parameterOffsets.push_back(intptr_t(parameter) - intptr_t(this));
  }

  void RootSignatureBase::AddSampler(const StaticSampler* sampler)
  {
    _samplerOffsets.push_back(intptr_t(sampler) - intptr_t(this));
  }

  winrt::com_ptr<ID3D12RootSignature> RootSignatureBase::Build() const
  {
    //Serialize root signature
    com_ptr<ID3DBlob> serializedRootSignature;
    {
      //Collect parameters
      vector<D3D12_ROOT_PARAMETER> parameters;
      parameters.reserve(_parameterOffsets.size());
      for (auto parameterOffset : _parameterOffsets)
      {
        auto parameter = reinterpret_cast<const RootParameter*>(intptr_t(this) + parameterOffset);
        parameters.push_back(parameter->Serialize());
      }

      //Collect samplers
      vector<D3D12_STATIC_SAMPLER_DESC> samplers;
      samplers.reserve(_samplerOffsets.size());
      for (auto samplerOffset : _samplerOffsets)
      {
        auto sampler = reinterpret_cast<const StaticSampler*>(intptr_t(this) + samplerOffset);
        samplers.push_back(sampler->Serialize());
      }

      //Create description
      D3D12_ROOT_SIGNATURE_DESC description{
        .NumParameters = uint32_t(parameters.size()),
        .pParameters = parameters.data(),
        .NumStaticSamplers = uint32_t(samplers.size()),
        .pStaticSamplers = samplers.data(),
        .Flags = D3D12_ROOT_SIGNATURE_FLAGS(Flags)
      };

      //Serialize description
      com_ptr<ID3DBlob> error;
      auto hresult = D3D12SerializeRootSignature(&description, D3D_ROOT_SIGNATURE_VERSION_1_0, serializedRootSignature.put(), error.put());      
      if (FAILED(hresult))
      {
        throw runtime_error(error ? reinterpret_cast<const char*>(error->GetBufferPointer()) : "Failed to serialize root signature!");
      }
    }

    //Create root signature
    com_ptr<ID3D12RootSignature> result;
    check_hresult(_device->CreateRootSignature(0u, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(result.put())));
    return result;
  }
}