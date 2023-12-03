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
      .SizeInBytes = _allocator.Size(),
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

  CommandFenceMarker ResourceUploader::EnqueueUploadTask(Resource* resource, TextureData* texture)
  {
    //Get description
    auto description = resource->Description();
    description.Flags = D3D12_RESOURCE_FLAG_NONE;

    //Select location in system memory
    auto allocationInfo = _device->GetResourceAllocationInfo(0, 0, &description);
    auto allocation = AllocateBuffer(allocationInfo.SizeInBytes, allocationInfo.Alignment);

    //Create upload task
    UploadTask task{
      .TargetResource = resource->_resource,
      .AllocatedSegment = allocation
    };

    //Create upload resource
    check_hresult(_device->CreatePlacedResource(_uploadHeap.get(), allocation.Start, &description, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(task.SourceResource.put())));

    //Copy data into system memory resource
    auto header = texture->Header();
    auto sliceCount = max(header.ArraySize, header.Depth);

    for (auto mip = 0u; mip < header.MipCount; mip++)
    {
      for (auto slice = 0u; slice < sliceCount; slice++)
      {
        auto subresourceIndex = GetSubresourceIndex(mip, slice, 0, header.MipCount, sliceCount);

        uint64_t rowPitch, slicePitch;
        auto bytes = texture->AsRawSpan(&rowPitch, slice, mip);
        slicePitch = bytes.size();

        check_hresult(task.SourceResource->Map(subresourceIndex, &EmptyRange, nullptr));
        check_hresult(task.SourceResource->WriteToSubresource(subresourceIndex, nullptr, bytes.data(), uint32_t(rowPitch), uint32_t(slicePitch)));
        task.SourceResource->Unmap(subresourceIndex, nullptr);
      }
    }

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
        allocator.ResourceTransition(task.SourceResource.get(), ResourceStates::Common, ResourceStates::CopySource);
        allocator.ResourceTransition(task.TargetResource.get(), ResourceStates::Common, ResourceStates::CopyDest);

        //Copy resource
        allocator->CopyResource(task.SourceResource.get(), task.TargetResource.get());

        //Set common state for later direct / compute engine use
        allocator.ResourceTransition(task.SourceResource.get(), ResourceStates::CopySource, ResourceStates::Common);
        allocator.ResourceTransition(task.TargetResource.get(), ResourceStates::CopyDest, ResourceStates::Common);
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
      _allocator.Deallocate(task.AllocatedSegment);
    }
  }

  BufferSegment ResourceUploader::AllocateBuffer(uint64_t size, uint64_t alignment)
  {
    BufferSegment result{};

    for (;;)
    {
      //Try allocating a buffer segment
      {
        lock_guard lock(_mutex);
        result = _allocator.TryAllocate(size, alignment);
      }

      //If the allocation succeeded we are done
      if (result) break;

      //Otherwise we wait for more space to become available
      _uploadEvent.wait();
    }

    return result;
  }
}