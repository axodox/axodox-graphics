#include "pch.h"
#include "RenderTargetView.h"
#include "../Commands/CommandAllocator.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const GraphicsDevice& device) :
    DescriptorHeap(device, DescriptorHeapKind::RenderTarget)
  { }
  
  RenderTargetViewRef RenderTargetDescriptorHeap::CreateRenderTargetView(Texture* texture)
  {
    return CreateDescriptor<RenderTargetView>(texture->get(), static_cast<D3D12_RENDER_TARGET_VIEW_DESC*>(nullptr));
  }

  void RenderTargetView::Clear(CommandAllocator& allocator, const DirectX::XMFLOAT4& value) const
  {
    allocator->ClearRenderTargetView(_handle, reinterpret_cast<const float*>(&value), 0, nullptr);
  }

  void RenderTargetView::Set(CommandAllocator& allocator) const
  {
    allocator->OMSetRenderTargets(1, &_handle, false, nullptr);

    D3D12_RECT scissorRect{ 0, 0, int32_t(_definition.Width), int32_t(_definition.Height) };
    allocator->RSSetScissorRects(1, &scissorRect);

    D3D12_VIEWPORT viewport{ 0, 0, float(_definition.Width), float(_definition.Height), 0.f, 1.f };
    allocator->RSSetViewports(1, &viewport);
  }

  const TextureDefinition& RenderTargetView::Definition() const
  {
    return _definition;
  }

  void RenderTargetView::OnInit()
  {
    _definition = _resource->GetDesc();
  }
}