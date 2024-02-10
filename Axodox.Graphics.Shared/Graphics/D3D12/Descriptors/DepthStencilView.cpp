#include "pch.h"
#include "DepthStencilView.h"
#include "../Commands/CommandAllocator.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  DepthStencilDescriptorHeap::DepthStencilDescriptorHeap(const GraphicsDevice& device) :
    DescriptorHeap(device, DescriptorHeapKind::DepthStencil)
  { }

  DepthStencilViewRef DepthStencilDescriptorHeap::CreateDepthStencilView(Texture* texture)
  {
    return CreateDescriptor<DepthStencilView>(texture->get(), static_cast<D3D12_DEPTH_STENCIL_VIEW_DESC*>(nullptr));
  }

  void DepthStencilView::Clear(CommandAllocator& allocator, float value) const
  {
    allocator->ClearDepthStencilView(_handle, D3D12_CLEAR_FLAG_DEPTH, value, 0, 0, nullptr);
  }

  void DepthStencilView::Set(CommandAllocator& allocator) const
  {
    allocator->OMSetRenderTargets(0, nullptr, true, &_handle);

    D3D12_RECT scissorRect{ 0, 0, int32_t(_definition.Width), int32_t(_definition.Height) };
    allocator->RSSetScissorRects(1, &scissorRect);

    D3D12_VIEWPORT viewport{ 0, 0, float(_definition.Width), float(_definition.Height), 0.f, 1.f };
    allocator->RSSetViewports(1, &viewport);
  }

  const TextureDefinition& DepthStencilView::Definition() const
  {
    return _definition;
  }

  void DepthStencilView::OnInit()
  {
    _definition = _resource->GetDesc();
  }
}