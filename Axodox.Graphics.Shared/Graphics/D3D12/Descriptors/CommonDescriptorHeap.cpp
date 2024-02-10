#include "pch.h"
#include "CommonDescriptorHeap.h"
#include "../Commands/CommandAllocator.h"

using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  CommonDescriptorHeap::CommonDescriptorHeap(const GraphicsDevice& device, uint32_t framesInFlight) :
    DescriptorHeap(device, DescriptorHeapKind::CommmonResource),
    _heaps(framesInFlight),
    _frameIndex(0u)
  { }

  ShaderResourceViewRef CommonDescriptorHeap::CreateShaderResourceView(Resource* resource)
  {
    return CreateDescriptor<ShaderResourceView>(resource->get(), static_cast<D3D12_SHADER_RESOURCE_VIEW_DESC*>(nullptr));
  }

  UnorderedAccessViewRef CommonDescriptorHeap::CreateUnorderedAccessView(Resource* resource)
  {
    return CreateDescriptor<UnorderedAccessView>(resource->get(), static_cast<D3D12_UNORDERED_ACCESS_VIEW_DESC*>(nullptr));
  }

  ConstantBufferViewRef CommonDescriptorHeap::CreateConstantBufferView(Buffer* resource)
  {
    D3D12_CONSTANT_BUFFER_VIEW_DESC description{
      .BufferLocation = (*resource)->GetGPUVirtualAddress(),
      .SizeInBytes = uint32_t(resource->Description().Width),
    };

    return CreateDescriptor<ConstantBufferView>(&description);
  }

  D3D12_GPU_DESCRIPTOR_HANDLE CommonDescriptorHeap::ResolveGpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
  {
    return { _handleBase.ptr + GetHandleOffset(handle) };
  }

  void CommonDescriptorHeap::Set(CommandAllocator& allocator)
  {
    auto heap = _heaps[_frameIndex].get();
    allocator->SetDescriptorHeaps(1, &heap);
  }

  void CommonDescriptorHeap::OnHeapBuilt(ID3D12DescriptorHeap* offlineHeap, uint32_t descriptorCount)
  {
    //Update frame index
    if (++_frameIndex == _heaps.size()) _frameIndex = 0;

    //Create descriptor heap
    auto& onlineHeap = _heaps[_frameIndex];
    if (!onlineHeap || onlineHeap->GetDesc().NumDescriptors < descriptorCount)
    {
      D3D12_DESCRIPTOR_HEAP_DESC description{
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = descriptorCount,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0u
      };

      check_hresult(_device->CreateDescriptorHeap(&description, IID_PPV_ARGS(onlineHeap.put())));
    }

    //Copy descriptors
    _device->CopyDescriptorsSimple(
      descriptorCount,
      onlineHeap->GetCPUDescriptorHandleForHeapStart(),
      offlineHeap->GetCPUDescriptorHandleForHeapStart(),
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //Set base descriptor
    _handleBase = onlineHeap->GetGPUDescriptorHandleForHeapStart();
  }
}