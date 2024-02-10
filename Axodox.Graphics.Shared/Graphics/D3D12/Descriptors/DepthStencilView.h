#pragma once
#include "SimpleResourceView.h"
#include "../Resources/Texture.h"

namespace Axodox::Graphics::D3D12
{
  class CommandAllocator;

  class AXODOX_GRAPHICS_API DepthStencilView : public SimpleResourceView<D3D12_DEPTH_STENCIL_VIEW_DESC, &ID3D12Device::CreateDepthStencilView>
  {
  public:
    using SimpleResourceView::SimpleResourceView;

    void Clear(CommandAllocator& allocator, float value = 1.f) const;
    void Set(CommandAllocator& allocator) const;

    const TextureDefinition& Definition() const;

  protected:
    virtual void OnInit() override;

  private:
    TextureDefinition _definition;
  };

  using DepthStencilViewRef = descriptor_ptr<DepthStencilView>;

  class AXODOX_GRAPHICS_API DepthStencilDescriptorHeap : public DescriptorHeap
  {
  public:
    DepthStencilDescriptorHeap(const GraphicsDevice& device);

    DepthStencilViewRef CreateDepthStencilView(Texture* texture);
  };
}