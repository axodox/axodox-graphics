#include "pch.h"
#include "Resource.h"
#include "ResourceAllocator.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  Resource::Resource(ResourceAllocator* owner) :
    _owner(owner),
    Allocated(_events)
  { }

  Resource::Resource(ID3D12Resource * resource) :
    Resource(com_ptr<ID3D12Resource>(resource, take_ownership_from_abi))
  { 
    resource->AddRef();
  }

  Resource::Resource(const winrt::com_ptr<ID3D12Resource>& resource) :
    _owner(nullptr),
    _resource(resource),
    Allocated(_events)
  { }

  const winrt::com_ptr<ID3D12Resource>& Resource::get() const
  {
    return _resource;
  }

  void Resource::set(winrt::com_ptr<ID3D12Resource>&& value)
  {
    _resource = move(value);
    _events.raise(Allocated, this);
  }

  ID3D12Resource* Resource::operator->() const
  {
    return _resource.get();
  }

  void ResourceDeleter::operator()(Resource* resource)
  {
    resource->_owner->DeleteResource(resource);
  }
}