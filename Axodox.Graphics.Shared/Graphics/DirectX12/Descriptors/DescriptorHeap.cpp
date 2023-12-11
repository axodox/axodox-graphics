#include "pch.h"
#include "DescriptorHeap.h"

using namespace Axodox::Infrastructure;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  DescriptorHeap::DescriptorHeap(const GraphicsDevice& device, DescriptorHeapKind type) :
    _device(device),
    _type(type),
    _handleBase{ 0 }
  { }

  DescriptorHeap::~DescriptorHeap()
  {
    Clean();

    assert(_items.empty());
  }

  DescriptorHeapKind DescriptorHeap::Type() const
  {
    return _type;
  }

  void DescriptorHeap::Build()
  {
    //Check dirty flag
    if (!_isDirty) return;

    //Clean descriptor references
    Clean();

    //Create descriptor heap
    uint32_t descriptorCount = uint32_t(_items.size());
    if (!_heap || _heap->GetDesc().NumDescriptors < descriptorCount)
    {
      D3D12_DESCRIPTOR_HEAP_DESC description{
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE(_type),
        .NumDescriptors = descriptorCount,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0u
      };

      check_hresult(_device->CreateDescriptorHeap(&description, IID_PPV_ARGS(_heap.put())));
      _handleBase = _heap->GetCPUDescriptorHandleForHeapStart();
    }

    //Realize descriptors
    auto increment = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(_type));
    auto handle = _heap->GetCPUDescriptorHandleForHeapStart();

    lock_guard lock(_mutex);
    for (auto& item : _items)
    {
      item->Realize(_device.get(), handle);
      handle.ptr += increment;
    }

    OnHeapBuilt(_heap.get(), descriptorCount);

    //Reset dirty flag
    _isDirty = false;
  }

  void DescriptorHeap::OnHeapBuilt(ID3D12DescriptorHeap* /*heap*/, uint32_t /*descriptorCount*/)
  { }

  int64_t DescriptorHeap::GetHandleOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
  {
    return handle.ptr - _handleBase.ptr;
  }

  void DescriptorHeap::DeleteDescriptor(const Descriptor* descriptor)
  {
    lock_guard lock(_mutex);
    _reclaimables.emplace(descriptor);
    _isDirty = true;
  }

  void DescriptorHeap::Clean()
  {
    lock_guard lock(_mutex);

    for (auto i = 0; i < _items.size(); i++)
    {
      auto& item = _items[i];
      if (_reclaimables.contains(item.get()))
      {
        swap(_items[i--], _items.back());
        _items.pop_back();
      }
    }

    _reclaimables.clear();
  }
}