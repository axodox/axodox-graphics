#include "pch.h"
#include "StaticSampler.h"
#include "RootSignature.h"

namespace Axodox::Graphics::D3D12
{
  TextureAddressMode3D::TextureAddressMode3D(TextureAddressMode mode) :
    AddressU(mode),
    AddressV(mode),
    AddressW(mode)
  { }

  TextureAddressMode3D::TextureAddressMode3D(TextureAddressMode modeU, TextureAddressMode modeV, TextureAddressMode modeW) :
    AddressU(modeU),
    AddressV(modeV),
    AddressW(modeW)
  { }

  StaticSampler::StaticSampler(RootSignatureMask* owner, InputSlot slot, ShaderVisibility visibility) :
    Slot(slot),
    Visibility(visibility)
  { 
    owner->AddSampler(this);
  }

  StaticSampler::StaticSampler(RootSignatureMask* owner, InputSlot slot, D3D12::Filter filter, TextureAddressMode addressMode, ShaderVisibility visibility) :
    Slot(slot),
    Filter(filter),
    AddressMode(addressMode),
    Visibility(visibility)
  {
    owner->AddSampler(this);
  }

  D3D12_STATIC_SAMPLER_DESC StaticSampler::Serialize() const
  {
    static_assert(sizeof(StaticSampler) == sizeof(D3D12_STATIC_SAMPLER_DESC));
    return reinterpret_cast<const D3D12_STATIC_SAMPLER_DESC&>(*this);
  }
}