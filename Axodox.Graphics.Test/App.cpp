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
using namespace Axodox::Storage;
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
    DynamicBufferManager DynamicBuffer;

    FrameResources(const GraphicsDevice& device) :
      Allocator(device),
      Fence(device),
      Marker(),
      DynamicBuffer(device)
    { }
  };

  struct SimpleRootDescription : public RootSignatureMask
  {
    RootDescriptor<RootDescriptorType::ConstantBuffer> ConstantBuffer;

    SimpleRootDescription(const RootSignatureContext& context) :
      RootSignatureMask(context),
      ConstantBuffer(this, { 0 }, ShaderVisibility::Vertex)
    {
      Flags = RootSignatureFlags::AllowInputAssemblerInputLayout;
    }
  };

  struct Constants
  {
    XMFLOAT4X4 WorldViewProjection;
  };

  void Run()
  {
    CoreWindow window = CoreWindow::GetForCurrentThread();
    window.Activate();

    CoreDispatcher dispatcher = window.Dispatcher();

    GraphicsDevice device{};
    CommandQueue directQueue{ device };
    CoreSwapChain swapChain{ directQueue, window };

    PipelineStateProvider pipelineStateProvider{ device };

    RootSignature<SimpleRootDescription> simpleRootSignature{ device };
    VertexShader simpleVertexShader{ app_folder() / L"SimpleVertexShader.cso" };
    PixelShader simplePixelShader{ app_folder() / L"SimplePixelShader.cso" };

    GraphicsPipelineStateDefinition simplePipelineStateDefinition{
      .RootSignature = &simpleRootSignature,
      .VertexShader = &simpleVertexShader,
      .PixelShader = &simplePixelShader,
      .InputLayout = VertexPositionNormal::Layout,
      .RenderTargetFormats = { Format::B8G8R8A8_UNorm }
    };
    auto simplePipelineState = pipelineStateProvider.CreatePipelineStateAsync(simplePipelineStateDefinition).get();

    array<FrameResources, 2> frames{ device, device };

    auto i = 0u;
    while (true)
    {
      //Process user input
      dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

      //Get frame resources
      auto& resources = frames[i++ & 0x1u];
      auto renderTargetView = swapChain.RenderTargetView();
      
      //Wait until buffers can be reused
      if (resources.Marker) resources.Fence.Await(resources.Marker);

      //Update constants
      Constants constants{};
      {
        auto resolution = swapChain.Resolution();
        auto projection = XMMatrixPerspectiveFovRH(90.f, float(resolution.x) / float(resolution.y), 0.01f, 10.f);
        auto view = XMMatrixLookAtRH(XMVectorSet(10.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f));
        auto world = XMMatrixIdentity();

        auto worldViewProjection = XMMatrixTranspose(world * view * projection);

        XMStoreFloat4x4(&constants.WorldViewProjection, worldViewProjection);
      }

      //Begin frame command list
      auto& allocator = resources.Allocator;
      {
        allocator.Reset();
        allocator.BeginList();
        allocator.ResourceTransition(*renderTargetView, ResourceStates::Present, ResourceStates::RenderTarget);
        renderTargetView->Clear(allocator, { sin(0.01f * i++), sin(0.01f * i++ + XM_2PI * 0.33f), sin(0.01f * i++ + XM_2PI * 0.66f), 0.f });
      }

      //Draw objects
      {
        auto mask = simpleRootSignature.Set(allocator, RootSignatureUsage::Graphics);
        mask.ConstantBuffer = resources.DynamicBuffer.AddBuffer(constants);
      }

      //End frame command list
      {
        allocator.ResourceTransition(*renderTargetView, ResourceStates::RenderTarget, ResourceStates::Present);
        auto drawCommandList = allocator.EndList();

        allocator.BeginList();
        resources.DynamicBuffer.UploadResources(allocator);
        auto initCommandList = allocator.EndList();
        
        directQueue.Execute(initCommandList);
        directQueue.Execute(drawCommandList);
        resources.Marker = resources.Fence.EnqueueSignal(directQueue);
      }

      //Present frame
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
