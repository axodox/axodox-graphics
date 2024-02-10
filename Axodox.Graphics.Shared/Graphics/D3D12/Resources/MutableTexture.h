#pragma once
#include "Texture.h"
#include "ResourceAllocationContext.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API MutableTexture
  {
  public:
    MutableTexture(const ResourceAllocationContext& context);
    MutableTexture(const ResourceAllocationContext& context, const TextureDefinition& definition);

    const TextureDefinition* Definition() const;

    ShaderResourceView* ShaderResource() const;
    RenderTargetView* RenderTarget() const;
    DepthStencilView* DepthStencil() const;
    UnorderedAccessView* UnorderedAccess() const;

    void Allocate(const TextureDefinition& definition);
    void Resize(uint32_t width, uint32_t height);
    void Reset();

    operator ResourceArgument() const;
    explicit operator bool() const;

  private:
    ResourceAllocationContext _context;

    TextureRef _texture;
    ShaderResourceViewRef _shaderResourceView;
    RenderTargetViewRef _renderTargetView;
    DepthStencilViewRef _depthStencilView;
    UnorderedAccessViewRef _unorderedAccessView;
    std::unique_ptr<TextureDefinition> _definition;

    Infrastructure::event_subscription _allocatedSubscription;

    void OnAllocated(Resource* resource);
  };
}