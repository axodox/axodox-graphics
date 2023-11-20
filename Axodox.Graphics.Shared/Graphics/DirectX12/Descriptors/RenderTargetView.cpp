#include "pch.h"
#include "RenderTargetView.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  RenderTargetDescriptor::RenderTargetDescriptor(const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description) :
    Resource(resource),
    Description(description ? make_unique<D3D12_RENDER_TARGET_VIEW_DESC>(*description) : nullptr)
  { }

  void RenderTargetDescriptor::Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    device->CreateRenderTargetView(Resource.get(), Description.get(), destination);
  }

  RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const GraphicsDevice& device) :
    DescriptorHeap(device, DescriptorHeapKind::RenderTarget)
  { }
  
  RenderTargetView RenderTargetDescriptorHeap::CreateRenderTargetView(const winrt::com_ptr<ID3D12Resource>&resource, const D3D12_RENDER_TARGET_VIEW_DESC * description)
  {
    return CreateDescriptor<RenderTargetView>(resource, description);
  }

  ID3D12Resource* RenderTargetView::Resource() const
  {
    return _descriptor->Resource.get();
  }
}