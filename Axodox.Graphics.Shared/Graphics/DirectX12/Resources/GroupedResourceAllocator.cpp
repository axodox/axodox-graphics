#include "pch.h"
#include "GroupedResourceAllocator.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  void GroupedResourceAllocator::AllocateResources(ResourceSpan resources)
  {
    //Get allocation info & flags
    D3D12_RESOURCE_ALLOCATION_INFO heapAllocationInfo;
    D3D12_HEAP_FLAGS heapFlags;
    {
      bool hasBuffers = false;
      bool hasTextures = false;
      bool hasTargets = false;

      vector<D3D12_RESOURCE_DESC> descriptions;
      descriptions.reserve(resources.size());
      for (auto& resource : resources)
      {
        auto description = resource->Description();

        if (description.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
        {
          hasBuffers = true;
        }
        else
        {
          if (description.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
          {
            hasTargets = true;
          }
          else
          {
            hasTextures = true;
          }
        }

        descriptions.push_back(description);
      }

      heapAllocationInfo = _device->GetResourceAllocationInfo(0, uint32_t(descriptions.size()), descriptions.data());

      heapFlags = D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
      if (hasBuffers + hasTextures + hasTargets == 1)
      {
        if (!hasBuffers) heapFlags |= D3D12_HEAP_FLAG_DENY_BUFFERS;
        if (!hasTextures) heapFlags |= D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
        if (!hasTargets) heapFlags |= D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
      }
    }

    //Check if we need to create the heap
    bool isCreatingNewHeap;
    if (_heap)
    {
      auto description = _heap->GetDesc();
      isCreatingNewHeap =
        description.Alignment % heapAllocationInfo.Alignment != 0 ||
        description.SizeInBytes < heapAllocationInfo.SizeInBytes ||
        description.Flags != heapFlags;
    }
    else
    {
      isCreatingNewHeap = true;
    }

    //Create new heap if needed
    if (isCreatingNewHeap)
    {
      D3D12_HEAP_DESC description{
        .SizeInBytes = heapAllocationInfo.SizeInBytes,
        .Properties = {
          .Type = D3D12_HEAP_TYPE_DEFAULT,
          .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
          .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
          .CreationNodeMask = 0,
          .VisibleNodeMask = 0
        },
        .Alignment = heapAllocationInfo.Alignment,
        .Flags = heapFlags
      };

      check_hresult(_device->CreateHeap(&description, IID_PPV_ARGS(_heap.put())));
    }

    //Suballocate resources
    uint64_t offset = 0;
    for (auto& resource : resources)
    {
      //Get description
      auto description = resource->Description();

      //Apply memory alignment
      auto allocationInfo = _device->GetResourceAllocationInfo(0, 1, &description);

      auto alignmentRemainder = offset % allocationInfo.Alignment;
      if (alignmentRemainder != 0) offset += allocationInfo.Alignment - alignmentRemainder;
            
      //Create resource
      auto defaultClearValue = resource->DefaultClearValue();
      com_ptr<ID3D12Resource> allocation;
      check_hresult(_device->CreatePlacedResource(
        _heap.get(),
        offset,
        &description,
        resource->DefaultState(),
        defaultClearValue ? &*defaultClearValue : nullptr,
        IID_PPV_ARGS(allocation.put())));
      resource->set(move(allocation));

      //Increment offset by size
      offset += allocationInfo.SizeInBytes;
    }
  }
}