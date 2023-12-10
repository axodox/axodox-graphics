#include "pch.h"
#include "ResourceAllocator.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  ResourceAllocator::ResourceAllocator(const GraphicsDevice& device) :
    _device(device)
  { }

  resource_ptr<Texture> ResourceAllocator::CreateTexture(const TextureDefinition& definition)
  {
    auto resource = make_unique<Texture>(this, definition);
    auto result = resource_ptr<Texture>(resource.get());
    AddResource(move(resource));
    return result;
  }

  resource_ptr<Buffer> ResourceAllocator::CreateBuffer(const BufferDefinition& definition)
  {
    if (!definition.Length) return nullptr;

    auto resource = make_unique<Buffer>(this, definition);
    auto result = resource_ptr<Buffer>(resource.get());
    AddResource(move(resource));
    return result;
  }

  void ResourceAllocator::AddResource(std::unique_ptr<Resource>&& resource)
  {
    lock_guard lock(_mutex);
    _resources.push_back(move(resource));
    _isDirty = true;
  }

  void ResourceAllocator::DeleteResource(const Resource* resource)
  {
    lock_guard lock(_mutex);
    _reclaimables.emplace(resource);
    _isDirty = true;
  }

  void ResourceAllocator::Build()
  {
    //Check dirty flag
    if (!_isDirty) return;

    //Clean resource references
    Clean();

    //Allocate resources
    lock_guard lock(_mutex);
    AllocateResources(_resources);
  }

  void ResourceAllocator::Clean()
  {
    lock_guard lock(_mutex);

    for (auto i = 0; i < _resources.size(); i++)
    {
      auto& item = _resources[i];
      if (_reclaimables.contains(item.get()))
      {
        swap(_resources[i--], _resources.back());
        _resources.pop_back();
      }
    }

    _reclaimables.clear();
  }

  ResourceAllocator::~ResourceAllocator()
  {
    Clean();

    assert(_resources.empty());
  }
}