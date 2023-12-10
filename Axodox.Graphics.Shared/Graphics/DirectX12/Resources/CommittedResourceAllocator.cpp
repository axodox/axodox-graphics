#include "pch.h"
#include "CommittedResourceAllocator.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  void CommittedResourceAllocator::AllocateResources(ResourceSpan resources)
  {
    //Define heap properties
    D3D12_HEAP_PROPERTIES heapProperties{
      .Type = D3D12_HEAP_TYPE_DEFAULT,
      .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
      .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
      .CreationNodeMask = 0,
      .VisibleNodeMask = 0
    };

    //Create all resources which do not exist
    for (auto& resource : resources)
    {
      if(resource->get()) continue;

      auto description = resource->Description();

      auto defaultClearValue = resource->DefaultClearValue();
      com_ptr<ID3D12Resource> allocation;
      check_hresult(_device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &description,
        resource->DefaultState(),
        defaultClearValue ? &*defaultClearValue : nullptr,
        IID_PPV_ARGS(allocation.put())));

      resource->set(move(allocation));
    }
  }
}