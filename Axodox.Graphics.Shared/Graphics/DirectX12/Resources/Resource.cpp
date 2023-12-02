#include "pch.h"
#include "Resource.h"
#include "ResourceAllocator.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  Resource::Resource(ResourceAllocator* owner, const D3D12_RESOURCE_DESC& description) :
    _owner(owner),
    _description(move(description))
  { }

  const D3D12_RESOURCE_DESC& Resource::Description() const
  {
    return _description;
  }

  ID3D12Resource* Resource::get() const
  {
    return _resource.get();
  }

  ID3D12Resource** Resource::put()
  {
    return _resource.put();
  }

  void ResourceDeleter::operator()(Resource* resource)
  {
    resource->_owner->DeleteResource(resource);
  }
}