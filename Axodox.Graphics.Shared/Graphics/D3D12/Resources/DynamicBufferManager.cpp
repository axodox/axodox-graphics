#include "pch.h"
#include "DynamicBufferManager.h"

using namespace Axodox::Infrastructure;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  DynamicBufferManager::DynamicBufferManager(const GraphicsDevice& device, uint64_t defaultBlockSize) :
    _device(device),
    _defaultBlockSize(defaultBlockSize ? defaultBlockSize : 1024 * 1024)
  { }

  GpuVirtualAddress DynamicBufferManager::AddBuffer(std::span<const uint8_t> buffer)
  {
    lock_guard lock(_mutex);

    //Calculate required space
    auto requiredSpace = align_memory_offset(buffer.size(), 256);

    //Get a block to store the buffer
    auto block = GetOrCreateBlock(requiredSpace);

    //Calculate GPU reference
    auto gpuReference = block->GpuAddress + block->Position;

    //Copy data
    memcpy(block->WriteBuffer.data() + block->Position, buffer.data(), buffer.size());
    block->Position += requiredSpace;

    //Return reference
    return gpuReference;
  }

  void DynamicBufferManager::UploadResources(CommandAllocator& allocator)
  {
    lock_guard lock(_mutex);

    for (auto& block : _blocks)
    {
      //Skip blocks with no data
      if (block.Position == 0) continue;

      //Copy to upload buffer
      D3D12_RANGE writtenRange{ 0, block.Position };

      void* mappedBuffer = nullptr;
      check_hresult(block.UploadBuffer->Map(0, &EmptyRange, &mappedBuffer));
      memcpy(mappedBuffer, block.WriteBuffer.data(), uint32_t(block.Size));
      block.UploadBuffer->Unmap(0, &writtenRange);

      //Copy to default buffer
      allocator.TransitionResources({
        { block.UploadBuffer, ResourceStates::Common, ResourceStates::CopySource },
        { block.DefaultBuffer, ResourceStates::AllShaderResource, ResourceStates::CopyDest }
      });

      allocator->CopyResource(block.DefaultBuffer.get(), block.UploadBuffer.get());

      allocator.TransitionResources({
        { block.UploadBuffer, ResourceStates::CopySource, ResourceStates::Common },
        { block.DefaultBuffer, ResourceStates::CopyDest, ResourceStates::AllShaderResource }
      });

      //Reset block
      block.Position = 0;
    }
  }

  DynamicBufferManager::Block* DynamicBufferManager::GetOrCreateBlock(uint64_t requiredSpace)
  {
    //Allocate new block if needed
    if (_blocks.empty() || _blocks.back().Position + requiredSpace > _blocks.back().Size)
    {
      auto blockSize = max(_blocks.empty() ? _defaultBlockSize : _blocks.back().Size << 1, requiredSpace);
      _blocks.push_back(CreateNewBlock(blockSize));
    }

    return &_blocks.back();
  }

  DynamicBufferManager::Block DynamicBufferManager::CreateNewBlock(uint64_t size) const
  {
    Block block{
      .Size = size,
      .Position = 0
    };

    block.WriteBuffer.resize(size);

    //Define heap properties
    D3D12_HEAP_PROPERTIES heapProperties{
      .Type = D3D12_HEAP_TYPE_UPLOAD,
      .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
      .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
      .CreationNodeMask = 0,
      .VisibleNodeMask = 0
    };

    //Define resource description
    D3D12_RESOURCE_DESC resourceDescription{
      .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
      .Alignment = 0,
      .Width = size,
      .Height = 1,
      .DepthOrArraySize = 1,
      .MipLevels = 1,
      .Format = DXGI_FORMAT_UNKNOWN,
      .SampleDesc = { 1u, 0u },
      .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
      .Flags = D3D12_RESOURCE_FLAG_NONE
    };

    //Create upload buffer
    check_hresult(_device->CreateCommittedResource(
      &heapProperties, 
      D3D12_HEAP_FLAG_CREATE_NOT_ZEROED, 
      &resourceDescription, 
      D3D12_RESOURCE_STATE_COMMON, 
      nullptr, 
      IID_PPV_ARGS(block.UploadBuffer.put())));

    //Create default buffer
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    check_hresult(_device->CreateCommittedResource(
      &heapProperties,
      D3D12_HEAP_FLAG_CREATE_NOT_ZEROED,
      &resourceDescription,
      D3D12_RESOURCE_STATE_COMMON,
      nullptr,
      IID_PPV_ARGS(block.DefaultBuffer.put())));

    //Get address on GPU
    block.GpuAddress = block.DefaultBuffer->GetGPUVirtualAddress();

    return block;
  }
}