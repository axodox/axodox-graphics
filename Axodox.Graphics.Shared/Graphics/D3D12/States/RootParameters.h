#pragma once
#include "../Commands/CommandAllocator.h"
#include "../Descriptors/ShaderResourceView.h"
#include "RootSignatureMask.h"

namespace Axodox::Graphics::D3D12
{
  class RootSignatureMask;

  struct InputSlot
  {
    uint32_t Register = 0;
    uint32_t Space = 0;
  };

  enum class ShaderVisibility
  {
    All = D3D12_SHADER_VISIBILITY_ALL,
    Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
    Hull = D3D12_SHADER_VISIBILITY_HULL,
    Domain = D3D12_SHADER_VISIBILITY_DOMAIN,
    Geometry = D3D12_SHADER_VISIBILITY_GEOMETRY,
    Pixel = D3D12_SHADER_VISIBILITY_PIXEL,
    Amplification = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
    Mesh = D3D12_SHADER_VISIBILITY_MESH
  };

  enum class DescriptorRangeType
  {
    ShaderResource = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
    UnorderedAccess = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
    ConstantBuffer = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
    Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER
  };

  struct AXODOX_GRAPHICS_API DescriptorRange
  {
    DescriptorRangeType Type = DescriptorRangeType::ShaderResource;
    InputSlot Slot = { 0u, 0u };
    uint32_t Size = 1u;
    uint32_t Offset = 0u;

    operator D3D12_DESCRIPTOR_RANGE() const;
  };

  class AXODOX_GRAPHICS_API RootParameter
  {
  public:
    uint32_t Index;

    RootParameter(RootSignatureMask* owner, ShaderVisibility visibility);
    virtual D3D12_ROOT_PARAMETER Serialize() const = 0;

  protected:
    ShaderVisibility _visibility;
    RootSignatureContext* Context() const;

  private:
    ptrdiff_t _context;
  };

  template <typename T>
  class RootConstant : public RootParameter
  {
  public:
    RootConstant(RootSignatureMask* owner, InputSlot slot, ShaderVisibility visibility = ShaderVisibility::All) :
      RootParameter(owner, visibility),
      _slot(slot)
    { }

    virtual D3D12_ROOT_PARAMETER Serialize() const override
    {
      return D3D12_ROOT_PARAMETER{
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
        .Constants = {
          .Num32BitValues = Size(),
          .ShaderRegister = _slot.Register,
          .RegisterSpace = _slot.Space,
        },
        .ShaderVisibility = D3D12_SHADER_VISIBILITY(_visibility)
      };
    }

    void operator=(const T& value)
    {
      auto& allocator = *Context()->Allocator;
      switch (Context()->Usage)
      {
      case RootSignatureUsage::Graphics:
        allocator->SetGraphicsRoot32BitConstants(Index, Size(), AsBuffer(value).data(), 0);
        break;
      case RootSignatureUsage::Compute:
        allocator->SetComputeRoot32BitConstants(Index, Size(), AsBuffer(value).data(), 0);
        break;
      default:
        throw winrt::hresult_not_implemented();
      }
    }

  private:
    InputSlot _slot;

    static consteval uint32_t Size()
    {
      return uint32_t(size_of(T) / 4 + (size_of(T) % 4 == 0 ? 0 : 1));
    }

    static std::array<std::byte, Size()> AsBuffer(T value)
    {
      std::array<std::byte, Size()> buffer;

      buffer.fill(0);
      memcpy(buffer.data(), &value, sizeof(T));

      return buffer;
    }
  };

  enum class RootDescriptorType
  {
    ConstantBuffer = D3D12_ROOT_PARAMETER_TYPE_CBV,
    ShaderResource = D3D12_ROOT_PARAMETER_TYPE_SRV,
    UnorderedAccess = D3D12_ROOT_PARAMETER_TYPE_UAV
  };

  template<RootDescriptorType Type>
  class RootDescriptor : public RootParameter
  {
  public:
    RootDescriptor(RootSignatureMask* owner, InputSlot slot, ShaderVisibility visibility = ShaderVisibility::All) :
      RootParameter(owner, visibility),
      _slot(slot)
    { }

    virtual D3D12_ROOT_PARAMETER Serialize() const override
    {
      return D3D12_ROOT_PARAMETER{
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE(Type),
        .Descriptor = {
          .ShaderRegister = _slot.Register,
          .RegisterSpace = _slot.Space,
        },
        .ShaderVisibility = D3D12_SHADER_VISIBILITY(_visibility)
      };
    }

    void operator=(GpuVirtualAddress reference)
    {
      auto& allocator = *Context()->Allocator;
      switch (Context()->Usage)
      {
      case RootSignatureUsage::Graphics:
        switch (Type)
        {
        case RootDescriptorType::ConstantBuffer:
          allocator->SetGraphicsRootConstantBufferView(Index, reference);
          break;
        case RootDescriptorType::ShaderResource:
          allocator->SetGraphicsRootShaderResourceView(Index, reference);
          break;
        case RootDescriptorType::UnorderedAccess:
          allocator->SetGraphicsRootUnorderedAccessView(Index, reference);
          break;
        }
        break;

      case RootSignatureUsage::Compute:
        switch (Type)
        {
        case RootDescriptorType::ConstantBuffer:
          allocator->SetComputeRootConstantBufferView(Index, reference);
          break;
        case RootDescriptorType::ShaderResource:
          allocator->SetComputeRootShaderResourceView(Index, reference);
          break;
        case RootDescriptorType::UnorderedAccess:
          allocator->SetComputeRootUnorderedAccessView(Index, reference);
          break;
        }
        break;

      default:
        throw winrt::hresult_not_implemented();
      }
    }

  private:
    InputSlot _slot;
  };

  template<uint32_t Size>
  class RootDescriptorTable : public RootParameter
  {
  public:
    RootDescriptorTable(RootSignatureMask* owner, std::array<DescriptorRange, Size> ranges, ShaderVisibility visibility = ShaderVisibility::All) :
      RootParameter(owner, visibility)
    {
      for (auto i = 0u; i < Size; i++)
      {
        _ranges[i] = ranges[i];
      }
    }

    virtual D3D12_ROOT_PARAMETER Serialize() const override
    {
      return D3D12_ROOT_PARAMETER{
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        .DescriptorTable = {
          .NumDescriptorRanges = Size,
          .pDescriptorRanges = _ranges.data(),
        },
        .ShaderVisibility = D3D12_SHADER_VISIBILITY(_visibility)
      };
    }

    void operator=(GpuVirtualAddress reference)
    {
      auto& allocator = *Context()->Allocator;
      switch (Context()->Usage)
      {
      case RootSignatureUsage::Graphics:
        allocator->SetGraphicsRootDescriptorTable(Index, D3D12_GPU_DESCRIPTOR_HANDLE(reference));
        break;
      case RootSignatureUsage::Compute:
        allocator->SetComputeRootDescriptorTable(Index, D3D12_GPU_DESCRIPTOR_HANDLE(reference));
        break;
      default:
        throw winrt::hresult_not_implemented();
      }
    }

  private:
    std::array<D3D12_DESCRIPTOR_RANGE, Size> _ranges;
  };
}