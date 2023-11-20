#include "pch.h"
#include "RenderTargetView.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  RenderTargetView::RenderTargetView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description) :
    Descriptor(owner),
    _resource(resource),
    _description(description ? make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*description) : nullptr)
  { }

  void RenderTargetView::OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    device->CreateRenderTargetView(_resource.get(), _description.get(), destination);
  }

  ID3D12Resource* RenderTargetView::Resource() const
  {
    return _resource.get();
  }

  RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const GraphicsDevice& device) :
    DescriptorHeap(device, DescriptorHeapKind::RenderTarget)
  { }
  
  descriptor_ptr<RenderTargetView> RenderTargetDescriptorHeap::CreateRenderTargetView(const winrt::com_ptr<ID3D12Resource>&resource, const D3D12_RENDER_TARGET_VIEW_DESC * description)
  {
    return CreateDescriptor<RenderTargetView>(resource, description);
  }
}