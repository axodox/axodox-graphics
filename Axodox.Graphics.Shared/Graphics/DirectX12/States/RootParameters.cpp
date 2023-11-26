#include "pch.h"
#include "RootParameters.h"
#include "RootSignature.h"

namespace Axodox::Graphics::D3D12
{
  RootParameter::RootParameter(RootSignatureBase* owner, ShaderVisibility visibility) :
    Visibility(visibility)
  {
    owner->AddParameter(this);
  }
}
