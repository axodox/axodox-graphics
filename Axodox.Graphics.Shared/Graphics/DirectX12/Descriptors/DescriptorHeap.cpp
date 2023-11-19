#include "pch.h"
#include "DescriptorHeap.h"

using namespace std;
using namespace winrt;

namespace Axodox::Graphics::D3D12
{
  DescriptorHeap::DescriptorHeap(const GraphicsDevice& device, DescriptorHeapKind type) :
    _device(device),
    _type(type)
  { }

  DescriptorHeapKind DescriptorHeap::Type() const
  {
    return _type;
  }

  void DescriptorHeap::Build()
  {
    //Check dirty flag
    if (!_isDirty) return;

    //Pin & clean descriptor references
    auto items = PinAndClean();

    //Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC description{
      .Type = D3D12_DESCRIPTOR_HEAP_TYPE(_type),
      .NumDescriptors = uint32_t(items.size()),
      .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
      .NodeMask = 0u
    };

    check_hresult(_device->CreateDescriptorHeap(&description, guid_of<ID3D12DescriptorHeap>(), _heap.put_void()));

    //Realize descriptors
    auto increment = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(_type));
    auto handle = _heap->GetCPUDescriptorHandleForHeapStart();
    for (auto& item : items)
    {
      item->Realize(_device.get(), handle);
      item->Handle = handle;
      handle.ptr += increment;
    }

    //Reset dirty flag
    _isDirty = false;
  }

  void DescriptorHeap::AddDescriptor(const std::shared_ptr<Descriptor>& descriptor)
  {
    _isDirty = true;
    _items.push_back(descriptor);
  }

  std::vector<std::shared_ptr<Descriptor>> DescriptorHeap::PinAndClean()
  {
    vector<std::shared_ptr<Descriptor>> items;
    for (auto i = 0; i < _items.size(); i++)
    {
      auto locked = _items[i].lock();
      if (locked)
      {
        items.push_back(locked);
      }
      else
      {
        swap(_items[i--], _items.back());
        _items.pop_back();
      }
    }
    return items;
  }
}