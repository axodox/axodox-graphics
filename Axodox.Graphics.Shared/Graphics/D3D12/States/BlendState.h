#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  enum BlendType : uint8_t
  {
    Opaque,
    Additive,
    Subtractive,
    AlphaBlend,
    Minimum,
    Maximum
  };

  struct AXODOX_GRAPHICS_API BlendState
  {
    std::array<BlendType, 8> BlendTypes;

    BlendState(BlendType blendType = BlendType::Opaque);
    BlendState(std::initializer_list<BlendType> blendTypes);

    explicit operator D3D12_BLEND_DESC() const;
  };
}