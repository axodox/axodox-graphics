#pragma once
#include "../Devices/GraphicsDevice.h"
#include "../Commands/CommandAllocator.h"

namespace Axodox::Graphics::D3D12
{
  using BufferReference = D3D12_GPU_VIRTUAL_ADDRESS;

  class DynamicBufferManager
  {
    struct Block
    {
      winrt::com_ptr<ID3D12Resource> UploadBuffer, DefaultBuffer;
      std::vector<uint8_t, Collections::aligned_allocator<uint8_t>> WriteBuffer;
      uint64_t Size, Position;
      D3D12_GPU_VIRTUAL_ADDRESS GpuAddress;
    };

  public:
    DynamicBufferManager(const GraphicsDevice& device, uint64_t defaultBlockSize = 0);

    BufferReference AddBuffer(std::span<uint8_t*> data);

    void UploadResources(CommandAllocator& allocator);

  private:
    std::mutex _mutex;
    uint64_t _defaultBlockSize;
    GraphicsDevice _device;
    std::vector<Block> _blocks;
    
    Block* GetOrCreateBlock(uint64_t requiredSpace);
    Block CreateNewBlock(uint64_t size) const;
  };
}