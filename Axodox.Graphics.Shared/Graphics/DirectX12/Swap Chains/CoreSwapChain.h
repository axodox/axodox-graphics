#pragma once
#ifdef WINRT_Windows_UI_Core_H
#include "SwapChain.h"

namespace Axodox::Graphics::D3D12
{
  class CoreSwapChain : public SwapChain
  {
    typedef winrt::Windows::UI::Core::CoreWindow CoreWindow;

  public:
    CoreSwapChain(const CommandQueue& queue, const CoreWindow& window, SwapChainFlags flags = SwapChainFlags::Default);

    virtual DirectX::XMUINT2 GetSize() const override;
    virtual DXGI_MATRIX_3X2_F GetTransformation() const override;

  private:
    CoreWindow _window;
    CoreWindow::SizeChanged_revoker _sizeChangedRevoker;
  };
}
#endif