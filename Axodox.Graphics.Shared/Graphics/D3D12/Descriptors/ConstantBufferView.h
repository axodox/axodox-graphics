#pragma once
#include "DescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API ConstantBufferView : public Descriptor
  {
  public:
    ConstantBufferView(DescriptorHeap* owner, const D3D12_CONSTANT_BUFFER_VIEW_DESC* description = nullptr);

    ID3D12Resource* Resource() const;

  protected:
    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) override;

  private:
    winrt::com_ptr<ID3D12Resource> _resource;
    std::unique_ptr<D3D12_CONSTANT_BUFFER_VIEW_DESC> _description;
  };

  using ConstantBufferViewRef = descriptor_ptr<ConstantBufferView>;
}