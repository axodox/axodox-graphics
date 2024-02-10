#pragma once
#include "Resource.h"
#include "Texture.h"
#include "Buffer.h"
#include "../Devices/GraphicsDevice.h"

namespace Axodox::Graphics::D3D12
{
  using ResourceSpan = std::span<const std::unique_ptr<Resource>>;

  class AXODOX_GRAPHICS_API ResourceAllocator
  {
    friend struct ResourceDeleter;

  public:
    ResourceAllocator(const GraphicsDevice& device);

    [[nodiscard]] TextureRef CreateTexture(const TextureDefinition& definition);
    [[nodiscard]] BufferRef CreateBuffer(const BufferDefinition& definition);

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

    void AddResource(std::unique_ptr<Resource>&& resource);
    void DeleteResource(const Resource* resource);
  };
}