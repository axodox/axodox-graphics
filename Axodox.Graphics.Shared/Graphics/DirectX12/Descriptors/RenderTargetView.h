#pragma once
#include "DescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  class RenderTargetView : public Descriptor
  {
  public:
    RenderTargetView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description = nullptr);

    ID3D12Resource* Resource() const;

  protected:
    virtual void OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) override;

  private:
    winrt::com_ptr<ID3D12Resource> _resource;
    std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> _description;
  };

  class RenderTargetDescriptorHeap : public DescriptorHeap
  {
  public:
    RenderTargetDescriptorHeap(const GraphicsDevice& device);

    descriptor_ptr<RenderTargetView> CreateRenderTargetView(const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description = nullptr);
  };
}