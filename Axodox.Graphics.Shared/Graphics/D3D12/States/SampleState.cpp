#include "pch.h"
#include "SampleState.h"

namespace Axodox::Graphics::D3D12
{
  SampleState::operator DXGI_SAMPLE_DESC() const
  {
    return { Count, Quality };
  }
}