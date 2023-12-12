#pragma once
#include "Collections/ObjectPool.h"
#include "CommandKind.h"
#include "CommandList.h"
#include "../Devices/GraphicsDevice.h"
#include "../Descriptors/RenderTargetView.h"
#include "../Descriptors/DepthStencilView.h"
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
    void ResourceTransitions(std::span<ResourceReference> resources, ResourceStates from, ResourceStates to);
    
    void CopyResource(ResourceReference source, ResourceReference destination);

    void SetRenderTargets(std::initializer_list<const RenderTargetView*> renderTargets, const DepthStencilView* depthStencilView = nullptr);
    void Dispatch(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1);

    void Reset();

  private:
    GraphicsDevice _device;
    CommandKind _type;
    winrt::com_ptr<ID3D12CommandAllocator> _allocator;
    Collections::ObjectPool<CommandList> _lists;
    Collections::ObjectPoolHandle<CommandList> _recorder;
  };
}