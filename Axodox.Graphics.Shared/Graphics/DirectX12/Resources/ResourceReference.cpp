#include "pch.h"
#include "ResourceReference.h"

namespace Axodox::Graphics::D3D12
{
  ResourceReference::ResourceReference(ID3D12Resource* resource) :
    Pointer(resource)
  { }

  ResourceReference::ResourceReference(const winrt::com_ptr<ID3D12Resource>& resource) :
    Pointer(resource.get())
  { }
}