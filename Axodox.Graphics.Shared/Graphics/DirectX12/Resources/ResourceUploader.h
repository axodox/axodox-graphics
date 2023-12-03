#pragma once
#include "Resource.h"
#include "TextureDefinition.h"
#include "../Commands/CommandAllocator.h"
#include "../Commands/CommandFence.h"
#include "Infrastructure/BufferAllocator.h"

namespace Axodox::Graphics::D3D12
{
  class ResourceUploader
  {
    struct UploadTask
    {
      winrt::com_ptr<ID3D12Resource> SourceResource, TargetResource;
      Infrastructure::BufferSegment AllocatedSegment;
      CommandFenceMarker Marker;
    };

  public:
    ResourceUploader(const GraphicsDevice& device, uint64_t bufferSize = 0);

    CommandFenceMarker EnqueueUploadTask(Resource* resource, TextureData* texture);
    bool AwaitUploadTask(CommandFenceMarker marker, CommandFenceTimeout timeout = CommandFenceTimeout(INFINITE));

    winrt::fire_and_forget UploadResourcesAsync(CommandAllocator& allocator, uint64_t maxSize = 0);

  private:
    GraphicsDevice _device;

    std::mutex _mutex;
    std::queue<UploadTask> _uploadTasks;
    winrt::com_ptr<ID3D12Heap> _uploadHeap;
    Infrastructure::BufferAllocator _allocator;
    Threading::auto_reset_event _uploadEvent;
    
    CommandFence _fence;
    
    Infrastructure::BufferSegment AllocateBuffer(uint64_t size, uint64_t alignment);
  };
}