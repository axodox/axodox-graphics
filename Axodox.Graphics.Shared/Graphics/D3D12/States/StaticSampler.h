#pragma once
#include "RootParameters.h"

namespace Axodox::Graphics::D3D12
{
  enum class Filter
  {
    Point = D3D12_FILTER_MIN_MAG_MIP_POINT,
    Linear = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
    Anisotropic = D3D12_FILTER_ANISOTROPIC
  };

  enum class TextureAddressMode
  {
    Unknown,
    Wrap = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    Mirror = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
    Clamp = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    Border = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
    MirrorOnce = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
  };

  enum class StaticBorderColor
  {
    TransparentBlack = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
    OpaqueBlack = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
    OpaqueWhite = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
  };

  struct AXODOX_GRAPHICS_API TextureAddressMode3D
  {
    TextureAddressMode AddressU = TextureAddressMode::Clamp;
    TextureAddressMode AddressV = TextureAddressMode::Clamp;
    TextureAddressMode AddressW = TextureAddressMode::Clamp;

    TextureAddressMode3D(TextureAddressMode mode = TextureAddressMode::Clamp);

    TextureAddressMode3D(TextureAddressMode modeU, TextureAddressMode modeV, TextureAddressMode modeW);
  };

  struct AXODOX_GRAPHICS_API StaticSampler
  {
    Filter Filter = Filter::Linear;
    TextureAddressMode3D AddressMode{};
    float MipLodBias = 0.f;
    uint32_t MaxAnisotropy = 16;
    ComparisonFunction Comparison = ComparisonFunction::Unknown;
    StaticBorderColor BorderColor = StaticBorderColor::TransparentBlack;
    float MinLod = 0;
    float MaxLod = D3D12_FLOAT32_MAX;
    InputSlot Slot{};
    ShaderVisibility Visibility = ShaderVisibility::All;

    StaticSampler(RootSignatureMask* owner, InputSlot slot, ShaderVisibility visibility = ShaderVisibility::All);
    StaticSampler(RootSignatureMask* owner, InputSlot slot, D3D12::Filter filter, TextureAddressMode addressMode, ShaderVisibility visibility = ShaderVisibility::All);

    D3D12_STATIC_SAMPLER_DESC Serialize() const;
  };

}