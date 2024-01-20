#include "pch.h"
#include "ResourceArgument.h"

namespace Axodox::Graphics::D3D12
{
  ResourceArgument::ResourceArgument(ID3D12Resource* resource) :
    Pointer(resource)
  { }

  ResourceArgument::ResourceArgument(const winrt::com_ptr<ID3D12Resource>& resource) :
    Pointer(resource.get())
  { }
}