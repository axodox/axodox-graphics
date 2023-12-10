#include "pch.h"
#include "Buffer.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  Buffer::Buffer(ResourceAllocator* owner, const BufferDefinition& definition) :
    Resource(owner),
    _definition(definition)
  { }

  Buffer::Buffer(const winrt::com_ptr<ID3D12Resource>& resource) :
    Resource(resource),
    _definition(resource->GetDesc())
  { }

  const BufferDefinition& Buffer::Definition() const
  {
    return _definition;
  }

  D3D12_RESOURCE_DESC Buffer::Description() const
  {
    return D3D12_RESOURCE_DESC(_definition);
  }

  D3D12_RESOURCE_STATES Buffer::DefaultState() const
  {
    return D3D12_RESOURCE_STATE_COMMON;
  }

  std::optional<D3D12_CLEAR_VALUE> Buffer::DefaultClearValue() const
  {
    return nullopt;
  }
}