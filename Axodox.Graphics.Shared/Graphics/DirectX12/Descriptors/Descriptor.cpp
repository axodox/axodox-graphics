#include "pch.h"
#include "Descriptor.h"
#include "DescriptorHeap.h"

namespace Axodox::Graphics::D3D12
{
  Descriptor::Descriptor(DescriptorHeap* owner) :
    _owner(owner)
  { }

  void Descriptor::Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination)
  {
    _handle = destination;
    OnRealize(device, destination);
  }

  std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> Descriptor::Handle() const
  {
    return _handle;
  }

  Descriptor::operator bool() const
  {
    return _handle.has_value();
  }

  void DescriptorDeleter::operator()(Descriptor* descriptor)
  {
    descriptor->_owner->DeleteDescriptor(descriptor);
  }
}
