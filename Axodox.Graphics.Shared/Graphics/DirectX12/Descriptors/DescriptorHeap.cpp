#include "pch.h"
#include "DescriptorHeap.h"

using namespace Axodox::Infrastructure;
using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  DescriptorHeap::DescriptorHeap(const GraphicsDevice& device, DescriptorHeapKind type) :
    _device(device),
    _type(type)
  { }

  DescriptorHeap::~DescriptorHeap()
  {
    Clean();

    if (!_items.empty()) throw logic_error("Descriptor handles must be all released before the descriptor heap is deleted.");
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
    D3D12_DESCRIPTOR_HEAP_DESC description{
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE(_type),
      .NumDescriptors = uint32_t(_items.size()),
      .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
      .NodeMask = 0u
    };

    if (!_heap || !are_equal(_heap->GetDesc(), description))
    {
      check_hresult(_device->CreateDescriptorHeap(&description, guid_of<ID3D12DescriptorHeap>(), _heap.put_void()));
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

    //Reset dirty flag
    _isDirty = false;
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