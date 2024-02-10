#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  struct AXODOX_GRAPHICS_API SampleState
  {
    uint32_t Count = 1;
    uint32_t Quality = 0;

    explicit operator DXGI_SAMPLE_DESC() const;
  };
}