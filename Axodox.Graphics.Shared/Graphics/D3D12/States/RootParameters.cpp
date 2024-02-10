#include "pch.h"
#include "RootParameters.h"
#include "RootSignature.h"

namespace Axodox::Graphics::D3D12
{
  RootParameter::RootParameter(RootSignatureMask* owner, ShaderVisibility visibility) :
    _context(uintptr_t(this) - uintptr_t(&owner->_context)),
    _visibility(visibility)
  {
    owner->AddParameter(this);
  }

  RootSignatureContext* RootParameter::Context() const
  {
    return reinterpret_cast<RootSignatureContext*>(uintptr_t(this) - _context);
  }

  DescriptorRange::operator D3D12_DESCRIPTOR_RANGE() const
  {
    return D3D12_DESCRIPTOR_RANGE{
      .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE(Type),
      .NumDescriptors = Size,
      .BaseShaderRegister = Slot.Register,
      .RegisterSpace = Slot.Space,
      .OffsetInDescriptorsFromTableStart = Offset
    };
  }
}
