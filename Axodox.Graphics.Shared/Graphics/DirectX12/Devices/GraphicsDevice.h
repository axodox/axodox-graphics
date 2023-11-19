#pragma once
#include "../GraphicsTypes.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice
  {
  public:
    explicit GraphicsDevice(ID3D12DeviceChild* deviceChild);
    explicit GraphicsDevice(D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0);

    ID3D12DeviceT* get() const;
    ID3D12DeviceT* operator->() const;

  private:
    winrt::com_ptr<ID3D12DeviceT> _device;
  };
}