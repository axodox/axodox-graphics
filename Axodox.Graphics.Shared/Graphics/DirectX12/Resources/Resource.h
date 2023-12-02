#pragma once
#include "pch.h"
#include "Infrastructure/AnyPtr.h"

namespace Axodox::Graphics::D3D12
{
  class ResourceAllocator;

  class Resource
  {
    friend class ResourceAllocator;
    friend struct ResourceDeleter;

  public:
    Resource(ResourceAllocator* owner, const D3D12_RESOURCE_DESC& description);

    const D3D12_RESOURCE_DESC& Description() const;
    
    ID3D12Resource* get() const;
    ID3D12Resource** put();

  private:
    ResourceAllocator* _owner;
    D3D12_RESOURCE_DESC _description;
    Infrastructure::any_ptr _data;

    winrt::com_ptr<ID3D12Resource> _resource;
  };

  struct ResourceDeleter
  {
    void operator()(Resource* resource);
  };

  using resource_ptr = std::unique_ptr<Resource, ResourceDeleter>;
}