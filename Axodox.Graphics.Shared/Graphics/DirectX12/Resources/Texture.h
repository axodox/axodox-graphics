#pragma once
#include "Resource.h"
#include "TextureDefinition.h"

namespace Axodox::Graphics::D3D12
{
  class Texture : public Resource
  {
  public:
    Texture(ResourceAllocator* owner, const TextureDefinition& definition);
    Texture(const winrt::com_ptr<ID3D12Resource>& resource);

    const TextureDefinition& Definition() const;
    virtual D3D12_RESOURCE_DESC Description() const override;
    virtual D3D12_RESOURCE_STATES DefaultState() const override;
    virtual std::optional<D3D12_CLEAR_VALUE> DefaultClearValue() const override;

  private:
    TextureDefinition _definition;
  };
}