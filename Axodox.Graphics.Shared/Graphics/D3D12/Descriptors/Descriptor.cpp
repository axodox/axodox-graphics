#include "pch.h"
#include "Descriptor.h"
#include "DescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  Descriptor::Descriptor(DescriptorHeap* owner) :
    _owner(owner),
    _handle{ 0 }
  { }

  void Descriptor::Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    _handle = destination;
    OnRealize(device, destination);
  }

  D3D12_CPU_DESCRIPTOR_HANDLE Descriptor::CpuHandle() const
  {
    assert(_handle.ptr != 0);
    return _handle;
  }

  Descriptor::operator bool() const
  {
    return _handle.ptr != 0;
  }

  void DescriptorDeleter::operator()(Descriptor* descriptor)
  {
    descriptor->_owner->DeleteDescriptor(descriptor);
  }
}
