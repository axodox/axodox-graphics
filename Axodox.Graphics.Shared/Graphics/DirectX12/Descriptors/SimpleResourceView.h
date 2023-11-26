#pragma once
#include "DescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  template<typename DescriptionType>
  using CreateDescriptorViewFunc = void (ID3D12DeviceT::*)(ID3D12Resource*, const DescriptionType*, D3D12_CPU_DESCRIPTOR_HANDLE);

  template<typename DescriptionType, CreateDescriptorViewFunc<DescriptionType> CreateView>
  class SimpleResourceView : public Descriptor
  {
  public:
    SimpleResourceView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const DescriptionType* description = nullptr);

    ID3D12Resource* Resource() const;

  protected:
    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) override;

  private:
    winrt::com_ptr<ID3D12Resource> _resource;
    std::unique_ptr<DescriptionType> _description;
  };
  
}