#pragma once
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::DirectX12
{
  struct Descriptor
  {
    virtual ~Descriptor() = default;

    virtual void Realize(ID3D12DeviceT* device, D3D12_CPU_DESCRIPTOR_HANDLE destination) = 0;

    std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> Handle;
  };

  template<typename T>
  class DescriptorView
  {
  public:
    using descriptor_t = T;

    DescriptorView(std::shared_ptr<T>&& descriptor) :
      _descriptor(descriptor)
    { }

    std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> Handle() const
    {
      return _descriptor->Handle;
    }

  private:
    std::shared_ptr<Descriptor> _descriptor;
  };
}