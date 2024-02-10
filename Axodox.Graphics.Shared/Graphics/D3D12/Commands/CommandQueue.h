#pragma once
#include "CommandKind.h"
#include "CommandList.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice;

  class AXODOX_GRAPHICS_API CommandQueue
  {
  public:
    explicit CommandQueue(const GraphicsDevice& device, CommandKind type = CommandKind::Direct);

    GraphicsDevice Device() const;
    CommandKind Type() const;

    ID3D12CommandQueue* get() const;
    ID3D12CommandQueue* operator->() const;

    void Execute(CommandList& commandList);

  private:
    winrt::com_ptr<ID3D12CommandQueue> _queue;
    CommandKind _type;
  };
}