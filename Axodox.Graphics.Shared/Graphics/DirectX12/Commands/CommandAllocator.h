#pragma once
#include "Collections/ObjectPool.h"
#include "CommandKind.h"
#include "CommandList.h"
#include "../Devices/GraphicsDevice.h"
#include "../Descriptors/RenderTargetView.h"

namespace Axodox::Graphics::D3D12
{
  class GraphicsDevice;

  enum class ResourceStates
  {
    Common = D3D12_RESOURCE_STATE_COMMON,
    VertexAndConstantBuffer = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
    IndexBuffer = D3D12_RESOURCE_STATE_INDEX_BUFFER,
    RenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET,
    UnorderedAccess = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
    DepthWrite = D3D12_RESOURCE_STATE_DEPTH_WRITE,
    DepthRead = D3D12_RESOURCE_STATE_DEPTH_READ,
    NonPixelShaderResource = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
    PixelShaderResource = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
    StreamOut = D3D12_RESOURCE_STATE_STREAM_OUT,
    IndirectArgument = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
    CopyDest = D3D12_RESOURCE_STATE_COPY_DEST,
    CopySource = D3D12_RESOURCE_STATE_COPY_SOURCE,
    ResolveDest = D3D12_RESOURCE_STATE_RESOLVE_DEST,
    ResolveSource = D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
    RaytracingAccelerationStructure = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
    ShadingRateSource = D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE,
    GenericRead = D3D12_RESOURCE_STATE_GENERIC_READ,
    AllShaderResource = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
    Present = D3D12_RESOURCE_STATE_PRESENT,
    Predication = D3D12_RESOURCE_STATE_PREDICATION,
    VideoDecodeRead = D3D12_RESOURCE_STATE_VIDEO_DECODE_READ,
    VideoDecodeWrite = D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE,
    VideoProcessRead = D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ,
    VideoProcessWrite = D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE,
    VideoEncodeRead = D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ,
    VideoEncodeWrite = D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE
  };

  class CommandAllocator
  {
  public:
    CommandAllocator(const GraphicsDevice& device, CommandKind type = CommandKind::Direct);

    CommandAllocator(const CommandAllocator&) = delete;
    CommandAllocator& operator=(const CommandAllocator&) = delete;

    ID3D12GraphicsCommandListT* operator->();

    void BeginList();
    [[nodiscard]] Collections::ObjectPoolHandle<CommandList> EndList();

    CommandKind Type() const;

    void ResourceTransition(ID3D12Resource* resource, ResourceStates from, ResourceStates to);

    void Reset();

  private:
    GraphicsDevice _device;
    CommandKind _type;
    winrt::com_ptr<ID3D12CommandAllocator> _allocator;
    Collections::ObjectPool<CommandList> _lists;
    Collections::ObjectPoolHandle<CommandList> _recorder;
  };
}