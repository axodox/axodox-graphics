#include "pch.h"
#include "CommandQueue.h"
#include "../Devices/GraphicsDevice.h"

using namespace Axodox::Infrastructure;
using namespace winrt;

namespace Axodox::Graphics::DirectX12
{
  CommandQueue::CommandQueue(const GraphicsDevice& device, CommandQueueKind type) :
    _type(type)
  {
    D3D12_COMMAND_QUEUE_DESC description;
    zero_memory(description);

    description.Type = D3D12_COMMAND_LIST_TYPE(type);
    check_hresult(device->CreateCommandQueue(&description, guid_of<ID3D12CommandQueue>(), _queue.put_void()));
  }

  GraphicsDevice CommandQueue::Device() const
  {
    return GraphicsDevice{ _queue.get() };
  }

  CommandQueueKind CommandQueue::Type() const
  {
    return _type;
  }

  ID3D12CommandQueue* CommandQueue::get() const
  {
    return _queue.get();
  }

  ID3D12CommandQueue* CommandQueue::operator->() const
  {
    return _queue.get();
  }
}