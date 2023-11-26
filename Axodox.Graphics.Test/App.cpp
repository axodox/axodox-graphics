#include "pch.h"
#include "Include/Axodox.Graphics.DirectX12.h"

using namespace std;
using namespace winrt;

using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;

using namespace Axodox::Graphics::D3D12;
using namespace DirectX;

struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
  IFrameworkView CreateView()
  {
    return *this;
  }

  void Initialize(CoreApplicationView const&)
  {
  }

  void Load(hstring const&)
  {
  }

  void Uninitialize()
  {
  }

  struct FrameResources
  {
    CommandAllocator Allocator;
    CommandFence Fence;
    CommandFenceMarker Marker;

    FrameResources(const GraphicsDevice& device) :
      Allocator(device),
      Fence(device),
      Marker()
    { }
  };

  void Run()
  {
    CoreWindow window = CoreWindow::GetForCurrentThread();
    window.Activate();

    CoreDispatcher dispatcher = window.Dispatcher();

    GraphicsDevice device{};
    CommandQueue directQueue{ device };
    CoreSwapChain swapChain{ directQueue, window };

    array<FrameResources, 2> frames{ device, device };

    auto i = 0;
    while (true)
    {
      dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

      auto renderTargetView = swapChain.RenderTargetView();

      auto& resources = frames[i++ % 2];
      auto& allocator = resources.Allocator;
      if (resources.Marker) resources.Fence.Await(resources.Marker);
      allocator.Reset();

      allocator.BeginList();
      allocator.ResourceTransition(renderTargetView->Resource(), ResourceStates::Present, ResourceStates::RenderTarget);
      renderTargetView->Clear(allocator, { sin(0.01f * i++), sin(0.01f * i++ + XM_2PI * 0.33f), sin(0.01f * i++ + XM_2PI * 0.66f), 0.f });
      allocator.ResourceTransition(renderTargetView->Resource(), ResourceStates::RenderTarget, ResourceStates::Present);
      auto commandList = allocator.EndList();

      directQueue.Execute(commandList);
      resources.Marker = resources.Fence.EnqueueSignal(directQueue);

      swapChain.Present();
    }
  }

  void SetWindow(CoreWindow const& window)
  {

  }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
  CoreApplication::Run(make<App>());
}
