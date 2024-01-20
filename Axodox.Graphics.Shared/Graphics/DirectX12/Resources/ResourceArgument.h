#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  struct ResourceArgument
  {
    ResourceArgument(ID3D12Resource* resource);
    ResourceArgument(const winrt::com_ptr<ID3D12Resource>& resource);
  
    ID3D12Resource* Pointer;
  };
}