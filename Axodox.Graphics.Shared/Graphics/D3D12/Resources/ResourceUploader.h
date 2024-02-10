#pragma once
#include "Resource.h"
#include "TextureDefinition.h"
#include "BufferDefinition.h"
#include "../Commands/CommandAllocator.h"
#include "../Commands/CommandFence.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API ResourceUploader
  {
    struct UploadTask
    {
      winrt::com_ptr<ID3D12Resource> SourceResource, TargetResource;
      Infrastructure::buffer_segment AllocatedSegment;
      CommandFenceMarker Marker;
    };

  public:
    ResourceUploader(const GraphicsDevice& device, uint64_t bufferSize = 0);

    CommandFenceMarker EnqueueUploadTask(Resource* resource, const ResourceData* data);

    bool AwaitUploadTask(CommandFenceMarker marker, CommandFenceTimeout timeout = CommandFenceTimeout(INFINITE));

    Threading::async_action UploadResourcesAsync(CommandAllocator& allocator, uint64_t maxSize = 0);

  private:
    GraphicsDevice _device;

    std::mutex _mutex;
    std::queue<UploadTask> _uploadTasks;
    winrt::com_ptr<ID3D12Heap> _uploadHeap;
    Infrastructure::buffer_allocator _allocator;
    Threading::auto_reset_event _uploadEvent;
    
    CommandFence _fence;
    
    Infrastructure::buffer_segment AllocateBuffer(uint64_t size, uint64_t alignment);
  };
}