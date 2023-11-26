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
}