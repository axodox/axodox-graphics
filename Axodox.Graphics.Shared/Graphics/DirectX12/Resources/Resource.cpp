#include "pch.h"
#include "Resource.h"
#include "ResourceAllocator.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  Resource::Resource(
    ResourceAllocator* owner, 
    const D3D12_RESOURCE_DESC& description,
    uint64_t size, uint64_t alignment) :
    _owner(owner),
    _description(move(description)),
    _size(size),
    _alignment(alignment)
  { }

  const D3D12_RESOURCE_DESC& Resource::Description() const
  {
    return _description;
  }

  void ResourceDeleter::operator()(Resource* resource)
  {
    resource->_owner->DeleteResource(resource);
  }
}