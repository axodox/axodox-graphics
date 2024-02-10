#pragma once
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::D3D12
{
  enum class CapabilityFlags
  {
    None = 0,
    IsUniformMemoryAccess = 1,
    Unknown = -1
  };

  enum class MemoryPool
  {
    Unknown = D3D12_MEMORY_POOL_UNKNOWN,
    SystemMemory = D3D12_MEMORY_POOL_L0,
    VideoMemory = D3D12_MEMORY_POOL_L1
  };

  class AXODOX_GRAPHICS_API GraphicsDevice
  {
  public:
    explicit GraphicsDevice(ID3D12DeviceChild* deviceChild);
    explicit GraphicsDevice(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0);

    ID3D12DeviceT* get() const;
    ID3D12DeviceT* operator->() const;

    CapabilityFlags Capabilities();

    MemoryPool VideoMemoryPool();

  private:
    winrt::com_ptr<ID3D12DeviceT> _device;
    CapabilityFlags _flags = CapabilityFlags::Unknown;
  };
}