#include "pch.h"
#include "RootSignatureMask.h"
#include "RootSignature.h"
#include "RootParameters.h"

namespace Axodox::Graphics::D3D12
{
  RootSignatureMask::RootSignatureMask(const RootSignatureContext& context) :
    _context(context)
  { }

  void RootSignatureMask::AddParameter(RootParameter* parameter)
  {
    parameter->Index = uint32_t(_context.Blueprint->Parameters.size());
    _context.Blueprint->Parameters.push_back(parameter);
  }

  void RootSignatureMask::AddSampler(StaticSampler* sampler)
  {
    _context.Blueprint->Samplers.push_back(sampler);
  }
}