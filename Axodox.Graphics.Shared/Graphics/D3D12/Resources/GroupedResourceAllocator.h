#pragma once
#include "ResourceAllocator.h"

namespace Axodox::Graphics::D3D12
{
  class AXODOX_GRAPHICS_API GroupedResourceAllocator : public ResourceAllocator
  {
  public:
    using ResourceAllocator::ResourceAllocator;

    virtual void AllocateResources(ResourceSpan resources) override;

  private:
    winrt::com_ptr<ID3D12Heap> _heap;
  };
}