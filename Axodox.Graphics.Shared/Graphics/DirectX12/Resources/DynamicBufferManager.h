#pragma once
#include "../Devices/GraphicsDevice.h"
#include "../Commands/CommandAllocator.h"
#include "../../../Infrastructure/BitwiseOperations.h"

namespace Axodox::Graphics::D3D12
{
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

    [[nodiscard]] DescriptorReference AddBuffer(std::span<const uint8_t> buffer);

    template<typename T>
    [[nodiscard]] DescriptorReference AddBuffer(const T& value)
    {
      return AddBuffer(Infrastructure::to_span(value));
    }

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