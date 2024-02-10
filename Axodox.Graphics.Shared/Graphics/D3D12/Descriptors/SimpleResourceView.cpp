#include "pch.h"
#include "SimpleResourceView.h"

namespace Axodox::Graphics::D3D12
{
  template<typename DescriptionType, CreateDescriptorViewFunc<DescriptionType> CreateView>
  SimpleResourceView<DescriptionType, CreateView>::SimpleResourceView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const DescriptionType* description) :
    Descriptor(owner),
    _resource(resource),
    _description(description ? std::make_unique<DescriptionType>(*description) : nullptr)
  { }

  template<typename DescriptionType, CreateDescriptorViewFunc<DescriptionType> CreateView>
  ID3D12Resource* SimpleResourceView<DescriptionType, CreateView>::Resource() const
  {
    return _resource.get();
  }

  template<typename DescriptionType, CreateDescriptorViewFunc<DescriptionType> CreateView>
  SimpleResourceView<DescriptionType, CreateView>::operator ResourceArgument() const
  {
    return _resource.get();
  }

  template<typename DescriptionType, CreateDescriptorViewFunc<DescriptionType> CreateView>
  void SimpleResourceView<DescriptionType, CreateView>::OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    (device->*CreateView)(_resource.get(), _description.get(), destination);

    OnInit();
  }

  template class SimpleResourceView<D3D12_RENDER_TARGET_VIEW_DESC, &ID3D12Device::CreateRenderTargetView>;
  template class SimpleResourceView<D3D12_DEPTH_STENCIL_VIEW_DESC, &ID3D12Device::CreateDepthStencilView>;
  template class SimpleResourceView<D3D12_SHADER_RESOURCE_VIEW_DESC, &ID3D12Device::CreateShaderResourceView>;
}