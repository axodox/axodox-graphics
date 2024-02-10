#pragma once
#include "pch.h"
#include "../Devices/GraphicsDevice.h"

namespace Axodox::Graphics::D3D12
{
  class ResourceAllocator;

  class AXODOX_GRAPHICS_API Resource
  {
    friend class ResourceAllocator;
    friend class ResourceUploader;
    friend struct ResourceDeleter;
    friend class CommittedResourceAllocator;

    Infrastructure::event_owner _events;

  public:
    Resource(ID3D12Resource* resource);
    Resource(const winrt::com_ptr<ID3D12Resource>& resource);

    virtual D3D12_RESOURCE_DESC Description() const = 0;
    virtual D3D12_RESOURCE_STATES DefaultState() const = 0;
    virtual std::optional<D3D12_CLEAR_VALUE> DefaultClearValue() const = 0;
    
    const winrt::com_ptr<ID3D12Resource>& get() const;
    void set(winrt::com_ptr<ID3D12Resource>&& value);

    ID3D12Resource* operator->() const;

    Infrastructure::event_publisher<Resource*> Allocated;

    virtual ~Resource() = default;

  protected:
    Resource();

    void AllocateCommitted(const GraphicsDevice& device);

  private:
    winrt::com_ptr<ID3D12Resource> _resource;
  };

  struct AXODOX_GRAPHICS_API ResourceDeleter
  {
    ResourceAllocator* Owner = nullptr;

    void operator()(Resource* resource);
  };

  template<typename T>
  using resource_ptr = std::unique_ptr<T, ResourceDeleter>;

  using ResourceRef = resource_ptr<Resource>;
}