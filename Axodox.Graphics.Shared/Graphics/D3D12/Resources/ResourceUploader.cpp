#include "pch.h"
#include "ResourceUploader.h"
#include "Threading/ThreadPool.h"

using namespace Axodox::Infrastructure;
using namespace Axodox::Threading;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  ResourceUploader::ResourceUploader(const GraphicsDevice& device, uint64_t bufferSize) :
    _device(device),
    _allocator(bufferSize > 0 ? bufferSize : 128 * 1024 * 1024),
    _fence(device)
  {
    D3D12_HEAP_DESC description{
      .SizeInBytes = _allocator.size(),
      .Properties = {
        .Type = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 0,
        .VisibleNodeMask = 0
      },
      .Alignment = 0,
      .Flags = D3D12_HEAP_FLAG_CREATE_NOT_ZEROED
    };

    check_hresult(_device->CreateHeap(&description, IID_PPV_ARGS(_uploadHeap.put())));
  }

  CommandFenceMarker ResourceUploader::EnqueueUploadTask(Resource* resource, const ResourceData* data)
  {
    //Get description
    auto description = resource->Description();
    description.Flags = D3D12_RESOURCE_FLAG_NONE;

    //Textures cannot be placed into upload heaps
    if (description.Dimension > D3D12_RESOURCE_DIMENSION_BUFFER)
    {
      uint64_t length;
      _device->GetCopyableFootprints(&description, 0u, description.MipLevels * description.DepthOrArraySize, 0ull, nullptr, nullptr, nullptr, &length);

      description = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0ull,
        .Width = length,
        .Height = 1u,
        .DepthOrArraySize = 1u,
        .MipLevels = 1u,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = { 1u, 0u },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE
      };
    }

    //Select location in system memory
    auto allocationInfo = _device->GetResourceAllocationInfo(0, 1, &description);
    auto allocation = AllocateBuffer(allocationInfo.SizeInBytes, allocationInfo.Alignment);

    //Create upload task
    UploadTask task{
      .TargetResource = resource->_resource,
      .AllocatedSegment = allocation
    };

    //Create upload resource
    check_hresult(_device->CreatePlacedResource(_uploadHeap.get(), allocation.Start, &description, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(task.SourceResource.put())));

    //Copy data into system memory resource
    data->CopyToResource(task.SourceResource.get());

    //Enqueue upload task and return marker
    lock_guard lock(_mutex);
    auto result = task.Marker = _fence.CreateMarker();
    _uploadTasks.emplace(move(task));
    return result;
  }

  bool ResourceUploader::AwaitUploadTask(CommandFenceMarker marker, CommandFenceTimeout timeout)
  {
    return _fence.Await(marker, timeout);
  }

  Threading::async_action ResourceUploader::UploadResourcesAsync(CommandAllocator& allocator, uint64_t maxSize)
  {
    //Schedule copy tasks
    CommandFenceMarker lastMarker;
    vector<UploadTask> tasksInFlight;
    {
      lock_guard lock(_mutex);
      tasksInFlight.reserve(_uploadTasks.size());

      //We take tasks until we run out of them or exceed the specified copy size limit
      uint64_t uploadedSize = 0;
      while ((maxSize == 0 || uploadedSize < maxSize) && !_uploadTasks.empty())
      {
        //Pop a task from the queue
        UploadTask task = move(_uploadTasks.front());
        _uploadTasks.pop();

        //Increase uploaded size
        uploadedSize += task.AllocatedSegment.Size;

        //Transition to copy states - assume common state after resource creation and direct / compute engine use
        allocator.TransitionResources({ 
          { task.SourceResource, ResourceStates::Common, ResourceStates::CopySource }, 
          { task.TargetResource, ResourceStates::Common, ResourceStates::CopyDest }
        });
        
        //Copy resource
        auto description = task.TargetResource->GetDesc();
        if (description.Dimension > D3D12_RESOURCE_DIMENSION_BUFFER)
        {
          vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts(description.DepthOrArraySize * description.MipLevels);
          _device->GetCopyableFootprints(&description, 0u, uint32_t(layouts.size()), 0ull, layouts.data(), nullptr, nullptr, nullptr);

          for (auto i = 0u; i < layouts.size(); i++)
          {
            D3D12_TEXTURE_COPY_LOCATION sourceLocation{
              .pResource = task.SourceResource.get(),
              .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
              .PlacedFootprint = layouts[i]
            };

            D3D12_TEXTURE_COPY_LOCATION targetLocation{
              .pResource = task.TargetResource.get(),
              .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
              .PlacedFootprint = i
            };

            allocator->CopyTextureRegion(&targetLocation, 0u, 0u, 0u, &sourceLocation, nullptr);
          }
        }
        else
        {
          allocator->CopyResource(task.TargetResource.get(), task.SourceResource.get());
        }

        //Set common state for later direct / compute engine use
        allocator.TransitionResources({
          { task.SourceResource, ResourceStates::CopySource, ResourceStates::Common },
          { task.TargetResource, ResourceStates::CopyDest, ResourceStates::Common }
        });
        lastMarker = task.Marker;

        //Move task to flight list
        tasksInFlight.push_back(move(task));
      }
    }

    //If we did nothing, return
    if (!lastMarker) co_return;

    //Otherwise add signaler and wait for it
    allocator.AddSignaler(lastMarker);
    co_await _fence.AwaitAsync(lastMarker);

    //Release resources    
    lock_guard lock(_mutex);
    for (auto& task : tasksInFlight)
    {
      _allocator.deallocate(task.AllocatedSegment);
    }
  }

  buffer_segment ResourceUploader::AllocateBuffer(uint64_t size, uint64_t alignment)
  {
    assert(size > 0ull);

    buffer_segment result{};

    for (;;)
    {
      //Try allocating a buffer segment
      {
        lock_guard lock(_mutex);
        result = _allocator.try_allocate(size, alignment);
      }

      //If the allocation succeeded we are done
      if (result) break;

      //Otherwise we wait for more space to become available
      _uploadEvent.wait();
    }

    return result;
  }
}