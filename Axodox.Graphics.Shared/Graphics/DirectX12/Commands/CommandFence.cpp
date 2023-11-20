#include "pch.h"
#include "CommandFence.h"
#include "../Devices/GraphicsDevice.h"

using namespace std;
using namespace winrt;

namespace {
  Axodox::Collections::ObjectPool<winrt::handle> _eventPool{ [] { return handle(CreateEvent({}, false, false, nullptr)); } };
}

namespace Axodox::Graphics::D3D12
{
  CommandFenceMarker::CommandFenceMarker() :
    Owner(nullptr),
    Value(0)
  { }

  CommandFenceMarker::CommandFenceMarker(CommandFence* owner, uint64_t value) :
    Owner(owner),
    Value(value)
  { }

  CommandFenceMarker::operator bool() const
  {
    return Owner != nullptr;
  }

  CommandFence::CommandFence(const GraphicsDevice& device) :
    _value(1)
  {
    check_hresult(device->CreateFence(
      0,
      D3D12_FENCE_FLAG_NONE,
      IID_PPV_ARGS(_fence.put())
    ));
  }

  CommandFenceMarker CommandFence::CreateMarker()
  {
    return { this, _value++ };
  }

  CommandFenceMarker CommandFence::EnqueueSignal(const CommandQueue& queue)
  {
    auto marker = CreateMarker();
    check_hresult(queue->Signal(_fence.get(), marker.Value));
    return marker;
  }

  void CommandFence::EnqueueSignal(const CommandQueue& queue, CommandFenceMarker marker) const
  {
    CheckMarker(marker);
    check_hresult(queue->Signal(_fence.get(), marker.Value));
  }

  CommandFenceMarker CommandFence::EnqueueAwait(const CommandQueue& queue)
  {
    auto marker = CreateMarker();
    check_hresult(queue->Wait(_fence.get(), marker.Value));
    return marker;
  }

  void CommandFence::EnqueueAwait(const CommandQueue& queue, CommandFenceMarker marker) const
  {
    CheckMarker(marker);
    check_hresult(queue->Wait(_fence.get(), marker.Value));
  }

  void CommandFence::Signal(CommandFenceMarker marker)
  {
    CheckMarker(marker);
    _fence->Signal(marker.Value);
  }

  bool CommandFence::Await(CommandFenceMarker marker, CommandFenceTimeout timeout)
  {
    CheckMarker(marker);

    auto isComplete = _fence->GetCompletedValue() >= marker.Value;
    if (isComplete || timeout == CommandFenceTimeout{0}) return isComplete;

    auto event = _eventPool.Borrow();
    check_hresult(_fence->SetEventOnCompletion(marker.Value, event->get()));

    return WaitForSingleObject(event->get(), timeout.count()) == WAIT_OBJECT_0;
  }

  void CommandFence::Sync(const CommandQueue& queue)
  {
    Await(EnqueueSignal(queue));
  }

  void CommandFence::CheckMarker(CommandFenceMarker marker) const
  {
    if (marker.Owner != this) throw logic_error("This marker does not belong to this fence!");
  }
}