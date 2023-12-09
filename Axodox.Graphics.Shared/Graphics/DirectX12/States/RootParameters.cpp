#include "pch.h"
#include "RootParameters.h"
#include "RootSignature.h"

namespace Axodox::Graphics::D3D12
{
  RootParameter::RootParameter(RootSignatureMask* owner, ShaderVisibility visibility) :
    _context(uintptr_t(this) - uintptr_t(&owner->_context)),
    Visibility(visibility)
  {
    owner->AddParameter(this);
  }

  RootSignatureContext* RootParameter::Context() const
  {
    return reinterpret_cast<RootSignatureContext*>(uintptr_t(this) - _context);
  }
}
