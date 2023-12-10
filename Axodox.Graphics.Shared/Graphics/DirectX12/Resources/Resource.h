#pragma once
#include "pch.h"
#include "Infrastructure/AnyPtr.h"

namespace Axodox::Graphics::D3D12
{
  class ResourceAllocator;

  class Resource
  {
    friend class ResourceAllocator;
    friend class ResourceUploader;
    friend struct ResourceDeleter;

    Infrastructure::event_owner _events;

  public:
    Resource(ResourceAllocator* owner, const D3D12_RESOURCE_DESC& description);

    const D3D12_RESOURCE_DESC& Description() const;
    
    ID3D12Resource* get() const;
    void set(winrt::com_ptr<ID3D12Resource>&& value);

    ID3D12Resource* operator->() const;

    Infrastructure::event_publisher<Resource*> Allocated;

  private:
    ResourceAllocator* _owner;
    D3D12_RESOURCE_DESC _description;
    winrt::com_ptr<ID3D12Resource> _resource;
  };

  struct ResourceDeleter
  {
    void operator()(Resource* resource);
  };

  using resource_ptr = std::unique_ptr<Resource, ResourceDeleter>;
}