#pragma once
#include "Collections/ObjectPool.h"
#include "CommandKind.h"
#include "CommandList.h"
#include "../Devices/GraphicsDevice.h"
#include "../Descriptors/RenderTargetView.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice;

  class CommandAllocator
  {
  public:
    CommandAllocator(const GraphicsDevice& device, CommandKind type = CommandKind::Direct);

    CommandAllocator(const CommandAllocator&) = delete;
    CommandAllocator& operator=(const CommandAllocator&) = delete;

    ID3D12GraphicsCommandListT* operator->();

    void BeginList();
    [[nodiscard]] Collections::ObjectPoolHandle<CommandList> EndList();

    CommandKind Type() const;

    void ResourceTransition(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to);
    void ClearRenderTargetView(const RenderTargetView* target, const DirectX::XMFLOAT4& value = { 0.f, 0.f, 0.f, 0.f });

    void Reset();

  private:
    GraphicsDevice _device;
    CommandKind _type;
    winrt::com_ptr<ID3D12CommandAllocator> _allocator;
    Collections::ObjectPool<CommandList> _lists;
    Collections::ObjectPoolHandle<CommandList> _recorder;
  };
}