#pragma once
#include "SimpleResourceView.h"
#include "../Commands/CommandAllocator.h"

namespace Axodox::Graphics::D3D12
{
  class RenderTargetView : public SimpleResourceView<D3D12_RENDER_TARGET_VIEW_DESC, &ID3D12Device::CreateRenderTargetView>
  {
  public:
    using SimpleResourceView::SimpleResourceView;

    void Clear(CommandAllocator& allocator, const DirectX::XMFLOAT4& value = { 0.f, 0.f, 0.f, 0.f }) const;
    void Set(CommandAllocator& allocator) const;

  protected:
    virtual void OnInit() override;

  private:
    int32_t _width, _height;
  };

  class RenderTargetDescriptorHeap : public DescriptorHeap
  {
  public:
    RenderTargetDescriptorHeap(const GraphicsDevice& device);

    descriptor_ptr<RenderTargetView> CreateRenderTargetView(const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* description = nullptr);
  };
}