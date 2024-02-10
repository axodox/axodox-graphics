#include "pch.h"
#ifdef WINRT_Windows_UI_Core_H
#include "CoreSwapChain.h"

using namespace Axodox::Threading;
using namespace DirectX;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  CoreSwapChain::CoreSwapChain(const CommandQueue& queue, const CoreWindow& window, SwapChainFlags flags) :
    SwapChain(queue, flags),
    _window(window)
  {
    //Create swap chain
    com_ptr<IDXGISwapChain1> swapChain;
    {
      auto swapChainDescription = CreateDescription();

      com_ptr<IDXGIFactory4> dxgiFactory;
      check_hresult(CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.put())));

      check_hresult(dxgiFactory->CreateSwapChainForCoreWindow(
        queue.get(),
        get_unknown(_window),
        &swapChainDescription,
        nullptr,
        swapChain.put()
      ));
    }

    //Subscribe to events
    _sizeChangedRevoker = window.SizeChanged(auto_revoke, [this](auto&, auto&) {
      _postPresentActions.invoke_async([this]() { Resize(); });
      });

    //Initialize swap chain
    InitializeSwapChain(swapChain);
  }

  DirectX::XMUINT2 CoreSwapChain::GetSize() const
  {
    return {};
  }

  DXGI_MATRIX_3X2_F CoreSwapChain::GetTransformation() const
  {
    return {};
  }
}
#endif
