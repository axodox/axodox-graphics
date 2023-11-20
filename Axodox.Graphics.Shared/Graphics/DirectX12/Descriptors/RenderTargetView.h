#pragma once
#include "DescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  struct RenderTargetDescriptor : public Descriptor
  {
    winrt::com_ptr<ID3D12Resource> Resource;
    std::unique_ptr<D3D12_RENDER_TARGET_VIEW_DESC> Description;

    RenderTargetDescriptor(const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description = nullptr);
    virtual void Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination);
  };

  class RenderTargetView : public DescriptorView<RenderTargetDescriptor>
  {
  public:
    using DescriptorView<RenderTargetDescriptor>::DescriptorView;

    ID3D12Resource* Resource() const;
  };

  class RenderTargetDescriptorHeap : public DescriptorHeap
  {
  public:
    RenderTargetDescriptorHeap(const GraphicsDevice& device);

    RenderTargetView CreateRenderTargetView(const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description = nullptr);
  };
}