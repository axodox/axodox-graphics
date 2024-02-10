#include "pch.h"
#include "UnorderedAccessView.h"
#include "CommonDescriptorHeap.h"
#include "../Resources/BufferDefinition.h"
#include "../Resources/TextureDefinition.h"

using namespace std;

namespace Axodox::Graphics::D3D12
{
  UnorderedAccessView::UnorderedAccessView(DescriptorHeap* owner, const winrt::com_ptr<ID3D12Resource>& resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* description) :
    Descriptor(owner),
    _resource(resource),
    _description(description ? std::make_unique<D3D12_UNORDERED_ACCESS_VIEW_DESC>(*description) : nullptr)
  { }

  void UnorderedAccessView::OnRealize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    device->CreateUnorderedAccessView(_resource.get(), nullptr, _description.get(), destination);

    auto description = _resource->GetDesc();
    if (description.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    {
      _definition = make_unique<BufferDefinition>(description);
    }
    else
    {
      _definition = make_unique<TextureDefinition>(description);
    }
  }

  const ResourceDefinition* UnorderedAccessView::Definition() const
  {
    return _definition.get();
  }

  D3D12_GPU_DESCRIPTOR_HANDLE UnorderedAccessView::GpuHandle() const
  {
    return static_cast<CommonDescriptorHeap*>(_owner)->ResolveGpuHandle(_handle);
  }

  UnorderedAccessView::operator GpuVirtualAddress() const
  {
    return GpuHandle().ptr;
  }

  ID3D12Resource* UnorderedAccessView::Resource() const
  {
    return _resource.get();
  }

  UnorderedAccessView::operator ResourceArgument() const
  {
    return _resource;
  }
}