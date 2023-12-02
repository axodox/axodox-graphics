#include "pch.h"
#include "CommittedResourceAllocator.h"

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
      auto data = GetResourceData<ResourceData>(resource.get());
      if (data->Resource) continue;

      check_hresult(_device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resource->Description(),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(data->Resource.put())));
    }
  }
}