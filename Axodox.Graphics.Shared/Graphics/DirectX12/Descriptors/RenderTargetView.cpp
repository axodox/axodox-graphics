#include "pch.h"
#include "RenderTargetView.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const GraphicsDevice& device) :
    DescriptorHeap(device, DescriptorHeapKind::RenderTarget)
  { }
  
  descriptor_ptr<RenderTargetView> RenderTargetDescriptorHeap::CreateRenderTargetView(const winrt::com_ptr<ID3D12Resource>&resource, const D3D12_RENDER_TARGET_VIEW_DESC * description)
  {
    return CreateDescriptor<RenderTargetView>(resource, description);
  }

  void RenderTargetView::Clear(CommandAllocator& allocator, const DirectX::XMFLOAT4& value) const
  {
    allocator->ClearRenderTargetView(*Handle(), reinterpret_cast<const float*>(&value), 0, nullptr);
  }

  void RenderTargetView::Set(CommandAllocator& allocator) const
  {
    auto handle = *Handle();
    allocator->OMSetRenderTargets(1, &handle, false, nullptr);

    D3D12_RECT scissorRect{ 0, 0, _width, _height };
    allocator->RSSetScissorRects(1, &scissorRect);

    D3D12_VIEWPORT viewport{ 0, 0, float(_width), float(_height), 0.f, 1.f };
    allocator->RSSetViewports(1, &viewport);
  }

  void RenderTargetView::OnInit()
  {
    auto description = _resource->GetDesc();
    _width = int32_t(description.Width);
    _height = int32_t(description.Height);
  }
}