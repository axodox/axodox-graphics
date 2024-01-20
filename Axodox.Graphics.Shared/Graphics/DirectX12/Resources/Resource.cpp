#include "pch.h"
#include "Resource.h"
#include "ResourceAllocator.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  Resource::Resource() :
    Allocated(_events)
  { }

  void Resource::AllocateCommitted(const GraphicsDevice& device)
  {
    //Define heap properties
    D3D12_HEAP_PROPERTIES heapProperties{
      .Type = D3D12_HEAP_TYPE_DEFAULT,
      .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
      .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
      .CreationNodeMask = 0,
      .VisibleNodeMask = 0
    };

    //Create resource
    auto description = Description();

    auto defaultClearValue = DefaultClearValue();
    com_ptr<ID3D12Resource> allocation;
    check_hresult(device->CreateCommittedResource(
      &heapProperties,
      D3D12_HEAP_FLAG_NONE,
      &description,
      DefaultState(),
      defaultClearValue ? &*defaultClearValue : nullptr,
      IID_PPV_ARGS(allocation.put())));

    set(move(allocation));
  }

  Resource::Resource(ID3D12Resource* resource) :
    Resource(com_ptr<ID3D12Resource>(resource, take_ownership_from_abi))
  {
    resource->AddRef();
  }

  Resource::Resource(const winrt::com_ptr<ID3D12Resource>& resource) :
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
    if (Owner) Owner->DeleteResource(resource);
  }
}