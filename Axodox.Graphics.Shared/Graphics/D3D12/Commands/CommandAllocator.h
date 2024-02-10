#pragma once
#include "pch.h"
#include "CommandKind.h"
#include "CommandList.h"
#include "../Devices/GraphicsDevice.h"
#include "../Descriptors/RenderTargetView.h"
#include "../Descriptors/DepthStencilView.h"
#include "../Resources/ResourceArgument.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice;
  class PipelineState;

  struct AXODOX_GRAPHICS_API ResourceTransition
  {
    ResourceArgument Resource;
    ResourceStates From, To;
  };

  class AXODOX_GRAPHICS_API CommandAllocator
  {
  public:
    CommandAllocator(const GraphicsDevice& device, CommandKind type = CommandKind::Direct);

    CommandAllocator(const CommandAllocator&) = delete;
    CommandAllocator& operator=(const CommandAllocator&) = delete;

    ID3D12GraphicsCommandListT* operator->();

    void BeginList(PipelineState* pipelineState = nullptr);
    [[nodiscard]] Collections::object_pool_handle<CommandList> EndList();

    CommandKind Type() const;

    void AddAwaiter(CommandFenceMarker marker);
    void AddSignaler(CommandFenceMarker marker);

    void TransitionResource(ResourceArgument resource, ResourceStates from, ResourceStates to);
    void TransitionResources(std::initializer_list<ResourceTransition> resources);
    
    void CopyResource(ResourceArgument source, ResourceArgument destination);

    void SetRenderTargets(std::initializer_list<const RenderTargetView*> renderTargets, const DepthStencilView* depthStencilView = nullptr);
    void Dispatch(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1);

    void Reset();

  private:
    GraphicsDevice _device;
    CommandKind _type;
    winrt::com_ptr<ID3D12CommandAllocator> _allocator;
    Collections::object_pool<CommandList> _lists;
    Collections::object_pool_handle<CommandList> _recorder;
  };
}