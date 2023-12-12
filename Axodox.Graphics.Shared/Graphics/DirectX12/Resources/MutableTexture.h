#pragma once
#include "Texture.h"
#include "ResourceAllocationContext.h"

namespace Axodox::Graphics::D3D12
{
  class MutableTexture
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

    operator ResourceReference() const;
    explicit operator bool() const;

  private:
    ResourceAllocationContext _context;

    resource_ptr<Texture> _texture;
    descriptor_ptr<ShaderResourceView> _shaderResourceView;
    descriptor_ptr<RenderTargetView> _renderTargetView;
    descriptor_ptr<DepthStencilView> _depthStencilView;
    descriptor_ptr<UnorderedAccessView> _unorderedAccessView;
    std::unique_ptr<TextureDefinition> _definition;

    Infrastructure::event_subscription _allocatedSubscription;

    void OnAllocated(Resource* resource);
  };
}