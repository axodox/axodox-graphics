#pragma once
#include "pch.h"

namespace Axodox::Graphics::DirectX12
{
  class GraphicsDevice;

  enum class CommandQueueKind
  {
    Direct = D3D12_COMMAND_LIST_TYPE_DIRECT,
    Compute = D3D12_COMMAND_LIST_TYPE_COMPUTE,
    Copy = D3D12_COMMAND_LIST_TYPE_COPY
  };

  class CommandQueue
  {
  public:
    explicit CommandQueue(const GraphicsDevice& device, CommandQueueKind type = CommandQueueKind::Direct);

    GraphicsDevice Device() const;
    CommandQueueKind Type() const;

    ID3D12CommandQueue* get() const;
    ID3D12CommandQueue* operator->() const;

  private:
    winrt::com_ptr<ID3D12CommandQueue> _queue;
    CommandQueueKind _type;
  };
}