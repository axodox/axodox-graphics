#pragma once
#include "DescriptorHeap.h"
#include "../Resources/ResourceArgument.h"

namespace Axodox::Graphics::D3D12
{
  template<typename DescriptionType>
  using CreateDescriptorViewFunc = void (ID3D12Device::*)(ID3D12Resource*, const DescriptionType*, D3D12_CPU_DESCRIPTOR_HANDLE);

  template<typename DescriptionType, CreateDescriptorViewFunc<DescriptionType> CreateView>
  class AXODOX_GRAPHICS_API SimpleResourceView : public Descriptor
  {
  public:
    SimpleResourceView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const DescriptionType* description = nullptr);

    ID3D12Resource* Resource() const;

    operator ResourceArgument() const;

  protected:
    winrt::com_ptr<ID3D12Resource> _resource;
    std::unique_ptr<DescriptionType> _description;

    virtual void OnInit() {};
    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) override;
  };
  
}