#include "pch.h"
#include "CommandAllocator.h"
#include "../States/PipelineState.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  CommandAllocator::CommandAllocator(const GraphicsDevice& device, CommandKind type) :
    _device(device),
    _type(type)
  { 
    check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE(type), IID_PPV_ARGS(_allocator.put())));
  }

  ID3D12GraphicsCommandListT* CommandAllocator::operator->()
  {
    if (!_recorder) BeginList();
    return _recorder->_list.get();
  }

  void CommandAllocator::BeginList(PipelineState* pipelineState)
  {
    if (_recorder) throw logic_error("Cannot start a command list before finishing the last one!");

    _recorder = _lists.borrow();
    if (!_recorder->_list)
    {
      check_hresult(_device->CreateCommandList(
        0u,
        D3D12_COMMAND_LIST_TYPE(_type),
        _allocator.get(),
        pipelineState ? pipelineState->get() : nullptr,
        IID_PPV_ARGS(_recorder->_list.put())
      ));
    }
    else
    {
      _recorder->_list->Reset(_allocator.get(), nullptr);
    }
  }

  Collections::object_pool_handle<CommandList> CommandAllocator::EndList()
  {
    if (!_recorder) throw logic_error("Cannot end a command list before starting one!");

    auto result{ move(_recorder) };
    result->_list->Close();
    return result;
  }

  CommandKind CommandAllocator::Type() const
  {
    return _type;
  }

  void CommandAllocator::AddAwaiter(CommandFenceMarker marker)
  {
    if (!_recorder) throw logic_error("There is no active command list!");
    _recorder->_awaiters.push_back(marker);
  }

  void CommandAllocator::AddSignaler(CommandFenceMarker marker)
  {
    if (!_recorder) throw logic_error("There is no active command list!");
    _recorder->_signalers.push_back(marker);
  }

  void CommandAllocator::TransitionResource(ResourceArgument resource, ResourceStates from, ResourceStates to)
  {
    D3D12_RESOURCE_BARRIER barrier{
      .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
      .Transition = { 
        .pResource = resource.Pointer,
        .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        .StateBefore = D3D12_RESOURCE_STATES(from),
        .StateAfter = D3D12_RESOURCE_STATES(to)
      }
    };

    (*this)->ResourceBarrier(1, &barrier);
  }

  void CommandAllocator::TransitionResources(std::initializer_list<ResourceTransition> resources)
  {
    vector<D3D12_RESOURCE_BARRIER> barriers;
    barriers.reserve(resources.size());

    for (auto resource : resources)
    {
      barriers.push_back({
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Transition = {
          .pResource = resource.Resource.Pointer,
          .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
          .StateBefore = D3D12_RESOURCE_STATES(resource.From),
          .StateAfter = D3D12_RESOURCE_STATES(resource.To)
        }
      });
    }

    (*this)->ResourceBarrier(uint32_t(barriers.size()), barriers.data());
  }

  void CommandAllocator::CopyResource(ResourceArgument source, ResourceArgument destination)
  {
    (*this)->CopyResource(destination.Pointer, source.Pointer);
  }

  void CommandAllocator::SetRenderTargets(std::initializer_list<const RenderTargetView*> renderTargets, const DepthStencilView* depthStencilView)
  {
    auto& definition = (*renderTargets.begin())->Definition();

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetHandles;
    renderTargetHandles.reserve(renderTargets.size());
    for (auto renderTarget : renderTargets)
    {
      renderTargetHandles.push_back(renderTarget->CpuHandle());
    }

    auto depthStencilHandle = depthStencilView ? depthStencilView->CpuHandle() : D3D12_CPU_DESCRIPTOR_HANDLE(0);
    (*this)->OMSetRenderTargets(uint32_t(renderTargetHandles.size()), renderTargetHandles.data(), false, &depthStencilHandle);

    D3D12_RECT scissorRect{ 0, 0, int32_t(definition.Width), int32_t(definition.Height) };
    (*this)->RSSetScissorRects(1, &scissorRect);

    D3D12_VIEWPORT viewport{ 0, 0, float(definition.Width), float(definition.Height), 0.f, 1.f };
    (*this)->RSSetViewports(1, &viewport);
  }

  void CommandAllocator::Dispatch(uint32_t x, uint32_t y, uint32_t z)
  {
    (*this)->Dispatch(x, y, z);
  }

  void CommandAllocator::Reset()
  {
    _allocator->Reset();
  }
}