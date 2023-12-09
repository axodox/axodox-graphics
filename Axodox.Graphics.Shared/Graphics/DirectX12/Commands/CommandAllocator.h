#pragma once
#include "Collections/ObjectPool.h"
#include "CommandKind.h"
#include "CommandList.h"
#include "../Devices/GraphicsDevice.h"
#include "../Descriptors/RenderTargetView.h"
#include "../Resources/ResourceReference.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice;
  class PipelineState;

  class CommandAllocator
  {
  public:
    CommandAllocator(const GraphicsDevice& device, CommandKind type = CommandKind::Direct);

    CommandAllocator(const CommandAllocator&) = delete;
    CommandAllocator& operator=(const CommandAllocator&) = delete;

    ID3D12GraphicsCommandListT* operator->();

    void BeginList(PipelineState* pipelineState = nullptr);
    [[nodiscard]] Collections::ObjectPoolHandle<CommandList> EndList();

    CommandKind Type() const;

    void AddAwaiter(CommandFenceMarker marker);
    void AddSignaler(CommandFenceMarker marker);

    void ResourceTransition(ResourceReference resource, ResourceStates from, ResourceStates to);

    void Reset();

  private:
    GraphicsDevice _device;
    CommandKind _type;
    winrt::com_ptr<ID3D12CommandAllocator> _allocator;
    Collections::ObjectPool<CommandList> _lists;
    Collections::ObjectPoolHandle<CommandList> _recorder;
  };
}