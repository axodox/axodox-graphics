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

  void Run()
  {
    CoreWindow window = CoreWindow::GetForCurrentThread();
    window.Activate();

    CoreDispatcher dispatcher = window.Dispatcher();
    
    GraphicsDevice device{};
    CommandQueue directQueue{ device };
    CoreSwapChain swapChain{ directQueue, window };
    CommandAllocator allocator(device);
    
    auto i = 0;
    while (true)
    {
      dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

      allocator.BeginList();
      allocator->ClearRenderTargetView(*swapChain.BackBuffer().Handle(), array<float, 4>{ sin(0.1f * i++), 0.f, 0.f, 0.f }.data(), 0, nullptr);
      auto commandList = allocator.EndList();

      directQueue.Execute(commandList);
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
