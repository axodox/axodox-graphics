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
using namespace Axodox::Infrastructure;
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

    MutableTexture DepthBuffer;
    MutableTexture PostProcessingBuffer;
    descriptor_ptr<ShaderResourceView> ScreenResourceView;

    FrameResources(const ResourceAllocationContext& context) :
      Allocator(*context.Device),
      Fence(*context.Device),
      Marker(),
      DynamicBuffer(*context.Device),
      DepthBuffer(context),
      PostProcessingBuffer(context)
    { }
  };

  struct SimpleRootDescription : public RootSignatureMask
  {
    RootDescriptor<RootDescriptorType::ConstantBuffer> ConstantBuffer;
    RootDescriptorTable<1> Texture;
    StaticSampler Sampler;

    SimpleRootDescription(const RootSignatureContext& context) :
      RootSignatureMask(context),
      ConstantBuffer(this, { 0 }, ShaderVisibility::Vertex),
      Texture(this, { DescriptorRangeType::ShaderResource }, ShaderVisibility::Pixel),
      Sampler(this, { 0 }, Filter::Linear, TextureAddressMode::Clamp, ShaderVisibility::Pixel)
    {
      Flags = RootSignatureFlags::AllowInputAssemblerInputLayout;
    }
  };

  struct PostProcessingRootDescription : public RootSignatureMask
  {
    RootDescriptor<RootDescriptorType::ConstantBuffer> ConstantBuffer;
    RootDescriptorTable<1> InputTexture;
    RootDescriptorTable<1> OutputTexture;
    StaticSampler Sampler;

    PostProcessingRootDescription(const RootSignatureContext& context) :
      RootSignatureMask(context),
      ConstantBuffer(this, { 0 }),
      InputTexture(this, { DescriptorRangeType::ShaderResource }),
      OutputTexture(this, { DescriptorRangeType::UnorderedAccess }),
      Sampler(this, { 0 }, Filter::Linear, TextureAddressMode::Clamp)
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
    CoreSwapChain swapChain{ directQueue, window, SwapChainFlags::IsShaderResource };

    PipelineStateProvider pipelineStateProvider{ device };

    RootSignature<SimpleRootDescription> simpleRootSignature{ device };
    VertexShader simpleVertexShader{ app_folder() / L"SimpleVertexShader.cso" };
    PixelShader simplePixelShader{ app_folder() / L"SimplePixelShader.cso" };

    GraphicsPipelineStateDefinition simplePipelineStateDefinition{
      .RootSignature = &simpleRootSignature,
      .VertexShader = &simpleVertexShader,
      .PixelShader = &simplePixelShader,
      .RasterizerState = RasterizerFlags::CullClockwise,
      .DepthStencilState = DepthStencilMode::WriteDepth,
      .InputLayout = VertexPositionNormalTexture::Layout,
      .RenderTargetFormats = { Format::B8G8R8A8_UNorm },
      .DepthStencilFormat = Format::D32_Float
    };
    auto simplePipelineState = pipelineStateProvider.CreatePipelineStateAsync(simplePipelineStateDefinition).get();

    RootSignature<PostProcessingRootDescription> postProcessingRootSignature{ device };
    ComputeShader postProcessingComputeShader{ app_folder() / L"PostProcessingComputeShader.cso" };
    ComputePipelineStateDefinition postProcessingStateDefinition{
      .RootSignature = &postProcessingRootSignature,
      .ComputeShader = &postProcessingComputeShader
    };
    auto postProcessingPipelineState = pipelineStateProvider.CreatePipelineStateAsync(postProcessingStateDefinition).get();

    GroupedResourceAllocator groupedResourceAllocator{ device };
    ResourceUploader resourceUploader{ device };
    CommonDescriptorHeap commonDescriptorHeap{ device, 2 };
    DepthStencilDescriptorHeap depthStencilDescriptorHeap{ device };
    ResourceAllocationContext immutableAllocationContext{
      .Device = &device,
      .ResourceAllocator = &groupedResourceAllocator,
      .ResourceUploader = &resourceUploader,
      .CommonDescriptorHeap = &commonDescriptorHeap,
      .DepthStencilDescriptorHeap = &depthStencilDescriptorHeap
    };

    ImmutableMesh cubeMesh{ immutableAllocationContext, CreateCube() };
    ImmutableTexture texture{ immutableAllocationContext, app_folder() / L"image.jpeg" };

    groupedResourceAllocator.Build();

    auto mutableDescriptorHeap = immutableAllocationContext;
    CommittedResourceAllocator committedResourceAllocator{ device };
    mutableDescriptorHeap.ResourceAllocator = &committedResourceAllocator;

    array<FrameResources, 2> frames{ mutableDescriptorHeap, mutableDescriptorHeap };

    swapChain.Resizing(no_revoke, [&](SwapChain*) {
      for (auto& frame : frames) frame.ScreenResourceView.reset();
      commonDescriptorHeap.Clean();
      });

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
      auto resolution = swapChain.Resolution();
      {
        auto projection = XMMatrixPerspectiveFovRH(90.f, float(resolution.x) / float(resolution.y), 0.01f, 10.f);
        auto view = XMMatrixLookAtRH(XMVectorSet(1.5f * cos(i * 0.002f), 1.5f * sin(i * 0.002f), 1.5f * cos(i * 0.0005f), 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f));
        auto world = XMMatrixIdentity();
        auto worldViewProjection = XMMatrixTranspose(world * view * projection);

        XMStoreFloat4x4(&constants.WorldViewProjection, worldViewProjection);
      }

      //Ensure depth buffer
      if (!resources.DepthBuffer || !TextureDefinition::AreSizeCompatible(*resources.DepthBuffer.Definition(), renderTargetView->Definition()))
      {
        auto depthDefinition = renderTargetView->Definition().MakeSizeCompatible(Format::D32_Float, TextureFlags::DepthStencil);
        resources.DepthBuffer.Allocate(depthDefinition);
      }

      //Ensure screen shader resource view
      if (!resources.ScreenResourceView || resources.ScreenResourceView->Resource() != renderTargetView->Resource())
      {
        Texture screenTexture{ renderTargetView->Resource() };
        resources.ScreenResourceView = commonDescriptorHeap.CreateShaderResourceView(&screenTexture);
      }

      //Ensure post processing buffer
      if (!resources.PostProcessingBuffer || !TextureDefinition::AreSizeCompatible(*resources.PostProcessingBuffer.Definition(), renderTargetView->Definition()))
      {
        auto postProcessingDefinition = renderTargetView->Definition().MakeSizeCompatible(Format::B8G8R8A8_UNorm, TextureFlags::UnorderedAccess);
        resources.PostProcessingBuffer.Allocate(postProcessingDefinition);
      }

      //Begin frame command list
      auto& allocator = resources.Allocator;
      {
        allocator.Reset();
        allocator.BeginList();
        allocator.ResourceTransition(*renderTargetView, ResourceStates::Present, ResourceStates::RenderTarget);

        committedResourceAllocator.Build();
        depthStencilDescriptorHeap.Build();

        commonDescriptorHeap.Build();
        commonDescriptorHeap.Set(allocator);

        renderTargetView->Clear(allocator, { sin(0.01f * i++), sin(0.01f * i++ + XM_2PI * 0.33f), sin(0.01f * i++ + XM_2PI * 0.66f), 0.f });
        resources.DepthBuffer.DepthStencil()->Clear(allocator);
      }

      //Draw objects
      {
        auto mask = simpleRootSignature.Set(allocator, RootSignatureUsage::Graphics);
        mask.ConstantBuffer = resources.DynamicBuffer.AddBuffer(constants);
        mask.Texture = texture;

        allocator.SetRenderTargets({ renderTargetView }, resources.DepthBuffer.DepthStencil());
        simplePipelineState.Apply(allocator);
        cubeMesh.Draw(allocator);
      }

      //Post processing
      {
        allocator.ResourceTransition(*renderTargetView, ResourceStates::RenderTarget, ResourceStates::NonPixelShaderResource);

        auto mask = postProcessingRootSignature.Set(allocator, RootSignatureUsage::Compute);
        mask.ConstantBuffer = resources.DynamicBuffer.AddBuffer(i * 0.02f);
        mask.InputTexture = *resources.ScreenResourceView;
        mask.OutputTexture = *resources.PostProcessingBuffer.UnorderedAccess();
        postProcessingPipelineState.Apply(allocator);

        auto definition = resources.PostProcessingBuffer.Definition();
        allocator.Dispatch(definition->Width / 16 + 1, definition->Height / 16 + 1);

        allocator.ResourceTransition(resources.PostProcessingBuffer, ResourceStates::UnorderedAccess, ResourceStates::CopySource);
        allocator.ResourceTransition(*renderTargetView, ResourceStates::NonPixelShaderResource, ResourceStates::CopyDest);
        allocator.CopyResource(resources.PostProcessingBuffer, *renderTargetView);
        allocator.ResourceTransition(resources.PostProcessingBuffer, ResourceStates::CopySource, ResourceStates::UnorderedAccess);
        allocator.ResourceTransition(*renderTargetView, ResourceStates::CopyDest, ResourceStates::RenderTarget);
      }

      //End frame command list
      {
        allocator.ResourceTransition(*renderTargetView, ResourceStates::RenderTarget, ResourceStates::Present);
        auto drawCommandList = allocator.EndList();

        allocator.BeginList();
        resources.DynamicBuffer.UploadResources(allocator);
        resourceUploader.UploadResourcesAsync(allocator);
        auto initCommandList = allocator.EndList();

        directQueue.Execute(initCommandList);
        directQueue.Execute(drawCommandList);
        resources.Marker = resources.Fence.EnqueueSignal(directQueue);
      }

      //Present frame
      swapChain.Present();
    }
  }

  void SetWindow(CoreWindow const& /*window*/)
  {

  }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
  CoreApplication::Run(make<App>());
}
