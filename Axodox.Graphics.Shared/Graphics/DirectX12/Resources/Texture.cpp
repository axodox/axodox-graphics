#include "pch.h"
#include "Texture.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Infrastructure;
using namespace std;

namespace Axodox::Graphics::D3D12
{
  Texture::Texture(const TextureDefinition& definition) :
    _definition(definition)
  { }

  Texture::Texture(const GraphicsDevice& device, const TextureDefinition& definition) :
    Texture(definition)
  {
    AllocateCommitted(device);
  }

  Texture::Texture(ID3D12Resource * resource) :
    Resource(resource),
    _definition(resource->GetDesc())
  { }

  Texture::Texture(const winrt::com_ptr<ID3D12Resource>& resource) :
    Resource(resource),
    _definition(resource->GetDesc())
  { }

  const TextureDefinition& Texture::Definition() const
  {
    return _definition;
  }

  D3D12_RESOURCE_DESC Texture::Description() const
  {
    return D3D12_RESOURCE_DESC(_definition);
  }

  D3D12_RESOURCE_STATES Texture::DefaultState() const
  {
    if (has_flag(_definition.Flags, TextureFlags::RenderTarget))
    {
      return D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    else if(has_flag(_definition.Flags, TextureFlags::DepthStencil))
    {
      return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }
    else if (has_flag(_definition.Flags, TextureFlags::UnorderedAccess))
    {
      return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    }
    else
    {
      return D3D12_RESOURCE_STATE_COMMON;
    }
  }

  std::optional<D3D12_CLEAR_VALUE> Texture::DefaultClearValue() const
  {
    if (has_flag(_definition.Flags, TextureFlags::RenderTarget))
    {
      return D3D12_CLEAR_VALUE{
        .Format = DXGI_FORMAT(_definition.PixelFormat),
        .Color = { 0.f, 0.f, 0.f, 0.f }
      };
    }
    else if (has_flag(_definition.Flags, TextureFlags::DepthStencil))
    {
      return D3D12_CLEAR_VALUE{
        .Format = DXGI_FORMAT(_definition.PixelFormat),
        .DepthStencil = { 1.f, 0 }
      };
    }
    else
    {
      return nullopt;
    }
  }
}