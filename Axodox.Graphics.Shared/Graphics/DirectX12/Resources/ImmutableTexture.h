#pragma once
#include "Texture.h"
#include "ResourceAllocationContext.h"

namespace Axodox::Graphics::D3D12
{
  class ImmutableTexture
  {
  public:
    ImmutableTexture(const ResourceAllocationContext& context, const std::filesystem::path& path);

    operator D3D12_GPU_VIRTUAL_ADDRESS() const;

  private:
    resource_ptr<Texture> _texture;
    descriptor_ptr<ShaderResourceView> _view;
    Infrastructure::event_subscription _allocatedSubscription;
  };
}