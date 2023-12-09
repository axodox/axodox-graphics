#include "pch.h"
#include "RootSignature.h"
#include "RootParameters.h"
#include "StaticSampler.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  RootSignatureBase::RootSignatureBase(const GraphicsDevice& device) :
    _device(device)
  { }

  ID3D12RootSignature* RootSignatureBase::get() const
  {
    return _signature.get();
  }

  void RootSignatureBase::SetSignature(RootSignatureMask* mask, CommandAllocator& allocator, RootSignatureUsage usage) const
  {
    auto& context = mask->_context;
    context.Blueprint = nullptr;
    context.Allocator = &allocator;
    context.Usage = usage;

    switch (context.Usage)
    {
    case RootSignatureUsage::Graphics:
      (*context.Allocator)->SetGraphicsRootSignature(_signature.get());
      break;
    case RootSignatureUsage::Compute:
      (*context.Allocator)->SetComputeRootSignature(_signature.get());
      break;
    default:
      throw hresult_not_implemented();
    }
  }

  winrt::com_ptr<ID3D12RootSignature> RootSignatureBase::BuildSignature(const RootSignatureMask* signature) const
  {
    //Serialize root signature
    com_ptr<ID3DBlob> serializedRootSignature;
    {
      auto blueprint = signature->_context.Blueprint;

      //Collect parameters
      vector<D3D12_ROOT_PARAMETER> parameters;
      parameters.reserve(blueprint->Parameters.size());
      for (auto parameter : blueprint->Parameters)
      {
        parameters.push_back(parameter->Serialize());
      }

      //Collect samplers
      vector<D3D12_STATIC_SAMPLER_DESC> samplers;
      samplers.reserve(blueprint->Samplers.size());
      for (auto sampler : blueprint->Samplers)
      {
        samplers.push_back(sampler->Serialize());
      }

      //Create description
      D3D12_ROOT_SIGNATURE_DESC description{
        .NumParameters = uint32_t(parameters.size()),
        .pParameters = parameters.data(),
        .NumStaticSamplers = uint32_t(samplers.size()),
        .pStaticSamplers = samplers.data(),
        .Flags = D3D12_ROOT_SIGNATURE_FLAGS(signature->Flags)
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