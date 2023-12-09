#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  struct ResourceReference
  {
    ResourceReference(ID3D12Resource* resource);
    ResourceReference(const winrt::com_ptr<ID3D12Resource>& resource);
  
    ID3D12Resource* Pointer;
  };
}