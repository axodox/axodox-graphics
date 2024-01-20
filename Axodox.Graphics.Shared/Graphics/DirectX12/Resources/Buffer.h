#pragma once
#include "Resource.h"
#include "BufferDefinition.h"

namespace Axodox::Graphics::D3D12
{
  class Buffer : public Resource
  {
  public:
    Buffer(ResourceAllocator* owner, const BufferDefinition& definition);
    Buffer(const winrt::com_ptr<ID3D12Resource>& resource);

    const BufferDefinition& Definition() const;
    virtual D3D12_RESOURCE_DESC Description() const override;
    virtual D3D12_RESOURCE_STATES DefaultState() const override;
    virtual std::optional<D3D12_CLEAR_VALUE> DefaultClearValue() const override;

  private:
    BufferDefinition _definition;
  };

  using BufferRef = resource_ptr<Buffer>;
}