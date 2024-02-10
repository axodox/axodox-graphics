#include "pch.h"
#include "SwapChain.h"
#include "Infrastructure/BitwiseOperations.h"
#include "../Devices/GraphicsDevice.h"

using namespace Axodox::Infrastructure;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  const uint32_t SwapChain::_minBufferSize = 64u;

  void SwapChain::Resize()
  {
    _fence.Sync(_queue);
    _targets.clear();
    _rtvHeap.Clean();

    _events.raise(Resizing, this);

    auto size = GetSize();
    check_hresult(_swapChain->ResizeBuffers(2, size.x, size.y, DXGI_FORMAT_UNKNOWN, has_flag(_flags, SwapChainFlags::IsTearingAllowed) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0));

    InitializeSwapChain(_swapChain);
  }

  void SwapChain::Present()
  {
    if (_marker) _fence.Await(_marker);

    if (has_flag(_flags, SwapChainFlags::IsTearingAllowed))
    {
      check_hresult(_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
    }
    else
    {
      check_hresult(_swapChain->Present(1, 0));
    }

    _marker = _fence.EnqueueSignal(_queue);
  }

  DirectX::XMUINT2 SwapChain::Resolution() const
  {
    return _resolution;
  }

  const RenderTargetView* SwapChain::RenderTargetView()
  {
    _postPresentActions.process_pending_invocations();
    return _targets[_swapChain->GetCurrentBackBufferIndex()].get();
  }

  SwapChain::SwapChain(const CommandQueue& queue, SwapChainFlags flags) :
    Resizing(_events),
    _rtvHeap(queue.Device()),
    _queue(queue),
    _fence(queue.Device()),
    _flags(flags)
  { }

  DXGI_SWAP_CHAIN_DESC1 SwapChain::CreateDescription() const
  {
    DXGI_SWAP_CHAIN_DESC1 result;
    zero_memory(result);

    if (has_flag(_flags, SwapChainFlags::IsTearingAllowed)) result.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    result.BufferCount = 2;
    result.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    result.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    if (has_flag(_flags, SwapChainFlags::IsShaderResource)) result.BufferUsage |= DXGI_USAGE_SHADER_INPUT;
    result.SampleDesc.Count = 1;
    result.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    auto size = GetSize();
    result.Width = size.x;
    result.Height = size.y;

    return result;
  }

  void SwapChain::InitializeSwapChain(const winrt::com_ptr<IDXGISwapChain>& swapChain)
  {
    //Store swap chain
    auto swapChain3 = swapChain.as<IDXGISwapChain3>();
    if (_swapChain != swapChain3)
    {
      _swapChain = swapChain3;
    }

    //Update targets
    if (_targets.empty())
    {
      for (auto i = 0; i < 2; i++)
      {
        com_ptr<ID3D12Resource> buffer;
        swapChain3->GetBuffer(i, IID_PPV_ARGS(buffer.put()));

        Texture texture{ buffer };
        _targets.push_back(_rtvHeap.CreateRenderTargetView(&texture));
      }

      auto description = _targets[0]->Resource()->GetDesc();
      _resolution = { uint32_t(description.Width), uint32_t(description.Height) };

      _rtvHeap.Build();
    }
    
    //Set transform
    auto swapChainTransform = GetTransformation();
    if (!is_default(swapChainTransform))
    {
      check_hresult(swapChain3->SetMatrixTransform(&swapChainTransform));
    }

    //Set color space
    check_hresult(swapChain3->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709));
  }
}
