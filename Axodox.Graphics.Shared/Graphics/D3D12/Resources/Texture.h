#pragma once
#include "Resource.h"
#include "TextureDefinition.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API Texture : public Resource
  {
    friend class ResourceAllocator;

  public:
    Texture(const GraphicsDevice& device, const TextureDefinition& definition);
    Texture(ID3D12Resource* resource);
    Texture(const winrt::com_ptr<ID3D12Resource>& resource);

    const TextureDefinition& Definition() const;
    virtual D3D12_RESOURCE_DESC Description() const override;
    virtual D3D12_RESOURCE_STATES DefaultState() const override;
    virtual std::optional<D3D12_CLEAR_VALUE> DefaultClearValue() const override;

  protected:
    Texture(const TextureDefinition& definition);

  private:
    TextureDefinition _definition;
  };

  using TextureRef = resource_ptr<Texture>;
}