#pragma once
#include "Resource.h"
#include "TextureDefinition.h"
#include "BufferDefinition.h"
#include "../Devices/GraphicsDevice.h"

namespace Axodox::Graphics::D3D12
{
  using ResourceSpan = std::span<const std::unique_ptr<Resource>>;

  class ResourceAllocator
  {
    friend struct ResourceDeleter;

  public:
    ResourceAllocator(const GraphicsDevice& device);

    [[nodiscard]] resource_ptr CreateTexture(const TextureDefinition& definition);
    [[nodiscard]] resource_ptr CreateBuffer(const BufferDefinition& definition);

    void Build();
    void Clean();

    virtual ~ResourceAllocator();

  protected:
    GraphicsDevice _device;

    virtual void AllocateResources(ResourceSpan resources) = 0;

  private:
    std::mutex _mutex;
    std::vector<std::unique_ptr<Resource>> _resources;
    std::set<const Resource*> _reclaimables;
    bool _isDirty = false;

    resource_ptr AddResource(const D3D12_RESOURCE_DESC& description);
    void DeleteResource(const Resource* resource);
  };
}