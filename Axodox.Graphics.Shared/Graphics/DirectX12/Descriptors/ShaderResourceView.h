#pragma once
#include "SimpleResourceView.h"
#include "../Resources/ResourceDefinition.h"

namespace Axodox::Graphics::D3D12
{
  class CommandAllocator;

  class ShaderResourceView : public SimpleResourceView<D3D12_SHADER_RESOURCE_VIEW_DESC, &ID3D12Device::CreateShaderResourceView>
  {
  public:
    using SimpleResourceView::SimpleResourceView;

    const ResourceDefinition* Definition() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle() const;
    operator D3D12_GPU_VIRTUAL_ADDRESS() const;

  protected:
    virtual void OnInit() override;

  private:
    std::unique_ptr<ResourceDefinition> _definition;
  };

  using ShaderResourceViewRef = descriptor_ptr<ShaderResourceView>;
}