#include "pch.h"
#include "ConstantBufferView.h"

namespace Axodox::Graphics::D3D12
{
  ConstantBufferView::ConstantBufferView(DescriptorHeap* owner, const D3D12_CONSTANT_BUFFER_VIEW_DESC* description) :
    Descriptor(owner),
    _description(description ? std::make_unique<D3D12_CONSTANT_BUFFER_VIEW_DESC>(*description) : nullptr)
  { }

  ID3D12Resource* ConstantBufferView::Resource() const
  {
    return _resource.get();
  }

  void ConstantBufferView::OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    device->CreateConstantBufferView(_description.get(), destination);
  }
}