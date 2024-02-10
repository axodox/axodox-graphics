#include "pch.h"
#include "MutableTexture.h"
#include "Infrastructure/BitwiseOperations.h"

using namespace Axodox::Infrastructure;
using namespace std;

namespace Axodox::Graphics::D3D12
{
  MutableTexture::MutableTexture(const ResourceAllocationContext& context) :
    _context(context)
  { }

  MutableTexture::MutableTexture(const ResourceAllocationContext & context, const TextureDefinition & definition) :
    MutableTexture(context)
  {
    Allocate(definition);
  }

  const TextureDefinition* MutableTexture::Definition() const
  {
    return _definition.get();
  }

  ShaderResourceView* MutableTexture::ShaderResource() const
  {
    return _shaderResourceView.get();
  }

  RenderTargetView* MutableTexture::RenderTarget() const
  {
    return _renderTargetView.get();
  }

  DepthStencilView* MutableTexture::DepthStencil() const
  {
    return _depthStencilView.get();
  }

  UnorderedAccessView* MutableTexture::UnorderedAccess() const
  {
    return _unorderedAccessView.get();
  }

  void MutableTexture::Allocate(const TextureDefinition& definition)
  {
    Reset();

    _texture = _context.ResourceAllocator->CreateTexture(definition);
    _allocatedSubscription = _texture->Allocated(event_handler{ this, &MutableTexture::OnAllocated });
  }

  void MutableTexture::Resize(uint32_t width, uint32_t height)
  {
    if (_definition && _definition->Width == width && _definition->Height == height) return;

    auto definition = *_definition;
    definition.Width = width;
    definition.Height = height;
    Allocate(definition);
  }

  void MutableTexture::Reset()
  {
    _texture.reset();
    _shaderResourceView.reset();
    _renderTargetView.reset();
    _depthStencilView.reset();
    _unorderedAccessView.reset();
    _definition.reset();
  }

  MutableTexture::operator ResourceArgument() const
  {
    return _texture->get();
  }

  MutableTexture::operator bool() const
  {
    return _texture != nullptr;
  }

  void MutableTexture::OnAllocated(Resource* resource)
  {
    _definition = make_unique<TextureDefinition>(resource->Description());
    auto flags = D3D12_RESOURCE_FLAGS(_definition->Flags);
    auto texture = static_cast<Texture*>(resource);

    if (!has_flag(flags, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE))
    {
      _shaderResourceView = _context.CommonDescriptorHeap->CreateShaderResourceView(resource);
    }

    if (has_flag(flags, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET))
    {
      _renderTargetView = _context.RenderTargetDescriptorHeap->CreateRenderTargetView(texture);
    }

    if (has_flag(flags, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL))
    {
      _depthStencilView = _context.DepthStencilDescriptorHeap->CreateDepthStencilView(texture);
    }

    if (has_flag(flags, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
    {
      _unorderedAccessView = _context.CommonDescriptorHeap->CreateUnorderedAccessView(texture);
    }
  }
}