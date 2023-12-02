#pragma once
#include "ResourceAllocator.h"

namespace Axodox::Graphics::D3D12
{
  class CommittedResourceAllocator : public ResourceAllocator
  {
    struct ResourceData
    {
      winrt::com_ptr<ID3D12Resource> Resource;
    };

  public:
    using ResourceAllocator::ResourceAllocator;

    virtual void AllocateResources(ResourceSpan resources) override;    
  };
}