#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  class ResourceAllocator;

  class Resource
  {
    friend class ResourceAllocator;
    friend struct ResourceDeleter;

  public:
    Resource(
      ResourceAllocator* owner, 
      const D3D12_RESOURCE_DESC& description, 
      uint64_t size, uint64_t alignment);

    const D3D12_RESOURCE_DESC& Description() const;

  private:
    ResourceAllocator* _owner;
    D3D12_RESOURCE_DESC _description;
    uint64_t _size, _alignment;
  };

  struct ResourceDeleter
  {
    void operator()(Resource* resource);
  };

  using resource_ptr = std::unique_ptr<Resource, ResourceDeleter>;
}