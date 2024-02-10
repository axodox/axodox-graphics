#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  class ResourceData
  {
  public:
    virtual void CopyToResource(ID3D12Resource* resource) const = 0;

    virtual ~ResourceData() = default;
  };
}