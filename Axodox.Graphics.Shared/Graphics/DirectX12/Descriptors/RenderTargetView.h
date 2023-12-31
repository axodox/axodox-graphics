#pragma once
#include "SimpleResourceView.h"
#include "../Resources/Texture.h"

namespace Axodox::Graphics::D3D12
{
  class CommandAllocator;

  class RenderTargetView : public SimpleResourceView<D3D12_RENDER_TARGET_VIEW_DESC, &ID3D12Device::CreateRenderTargetView>
  {
  public:
    using SimpleResourceView::SimpleResourceView;

    void Clear(CommandAllocator& allocator, const DirectX::XMFLOAT4& value = { 0.f, 0.f, 0.f, 0.f }) const;
    void Set(CommandAllocator& allocator) const;

    const TextureDefinition& Definition() const;

  protected:
    virtual void OnInit() override;

  private:
    TextureDefinition _definition;
  };

  class RenderTargetDescriptorHeap : public DescriptorHeap
  {
  public:
    RenderTargetDescriptorHeap(const GraphicsDevice& device);

    descriptor_ptr<RenderTargetView> CreateRenderTargetView(Texture* texture);
  };
}