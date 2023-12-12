#pragma once
#include "Descriptor.h"
#include "../Resources/ResourceReference.h"
#include "../Resources/ResourceDefinition.h"

namespace Axodox::Graphics::D3D12
{
  class UnorderedAccessView : public Descriptor
  {
  public:
    UnorderedAccessView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* description);

    const ResourceDefinition* Definition() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle() const;
    operator D3D12_GPU_VIRTUAL_ADDRESS() const;

    ID3D12Resource* Resource() const;
    operator ResourceReference() const;

  private:
    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) override;

    winrt::com_ptr<ID3D12Resource> _resource;
    std::unique_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC> _description;
    std::unique_ptr<ResourceDefinition> _definition;
  };
}