#pragma once
#include "../pch.h"
#include "../Commands/CommandAllocator.h"
#include "../Descriptors/ConstantBufferView.h"
#include "../Descriptors/ShaderResourceView.h"

namespace Axodox::Graphics::D3D12
{
  class RootSignatureBase;

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

  struct DescriptorRange
  {
    DescriptorRangeType Type;
    uint32_t Size;
    InputSlot Slot;
    uint32_t Offset;
  };

  static_assert(sizeof(D3D12_DESCRIPTOR_RANGE) == sizeof(DescriptorRange));

  struct RootParameter
  {
    ShaderVisibility Visibility;
    uint32_t Index;

    RootParameter(RootSignatureBase* owner, ShaderVisibility visibility);

    virtual D3D12_ROOT_PARAMETER Serialize() const = 0;
  };

  template <typename T>
  struct RootConstant : public RootParameter
  {
    InputSlot Slot;

    static consteval uint32_t Size()
    {
      return uint32_t(size_of(T) / 4 + (size_of(T) % 4 == 0 ? 0 : 1));
    }

    RootConstant(RootSignatureBase* owner, InputSlot slot, ShaderVisibility visibility = ShaderVisibility::All) :
      RootParameter(owner, visibility),
      Slot(slot)
    { }

    virtual D3D12_ROOT_PARAMETER Serialize() const override
    {
      D3D12_ROOT_PARAMETER result;
      result.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
      result.Constants.Num32BitValues = Size();
      result.Constants.ShaderRegister = Slot.Register;
      result.Constants.RegisterSpace = Slot.Space;
      result.ShaderVisibility = D3D12_SHADER_VISIBILITY(Visibility);
      return result;
    }

    static std::array<std::byte, Size()> AsBuffer(T value)
    {
      std::array<std::byte, Size()> buffer;

      buffer.fill(0);
      memcpy(buffer.data(), &value, sizeof(T));

      return buffer;
    }

    void SetGraphics(CommandAllocator& allocator, T value)
    {
      allocator->SetGraphicsRoot32BitConstants(Index, Size(), AsBuffer(value).data(), 0);
    }

    void SetCompute(CommandAllocator& allocator, T value)
    {
      std::array<std::byte, Size()> buffer;

      buffer.fill(0);
      memcpy(buffer.data(), &value, sizeof(T));

      allocator->SetComputeRoot32BitConstants(Index, Size(), AsBuffer(value).data(), 0);
    }
  };

  enum class RootDescriptorType
  {
    ConstantBuffer = D3D12_ROOT_PARAMETER_TYPE_CBV,
    ShaderResource = D3D12_ROOT_PARAMETER_TYPE_SRV,
    UnorderedAccess = D3D12_ROOT_PARAMETER_TYPE_UAV
  };

  template<RootDescriptorType Type>
  struct RootDescriptor : public RootParameter
  {
    InputSlot Slot;

    RootDescriptor(RootSignatureBase* owner, InputSlot slot, ShaderVisibility visibility = ShaderVisibility::All) :
      RootParameter(owner, visibility),
      Slot(slot)
    { }

    virtual D3D12_ROOT_PARAMETER Serialize() const override
    {
      D3D12_ROOT_PARAMETER result;
      result.ParameterType = D3D12_ROOT_PARAMETER_TYPE(Type);
      result.Descriptor.ShaderRegister = Slot.Register;
      result.Descriptor.RegisterSpace = Slot.Space;
      result.ShaderVisibility = D3D12_SHADER_VISIBILITY(Visibility);
      return result;
    }

    void SetGraphics(CommandAllocator& allocator, BufferReference reference)
    {
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
    }

    void SetCompute(CommandAllocator& allocator, BufferReference reference)
    {
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
    }
  };

  template<uint32_t Size>
  struct RootDescriptorTable : public RootParameter
  {
    std::array<DescriptorRange, Size> Ranges;

    RootDescriptorTable(RootSignatureBase* owner, std::array<DescriptorRange, Size> ranges, ShaderVisibility visibility = ShaderVisibility::All) :
      RootParameter(owner, visibility),
      Ranges(ranges)
    { }

    virtual D3D12_ROOT_PARAMETER Serialize() const override
    {
      D3D12_ROOT_PARAMETER result;
      result.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      result.DescriptorTable.NumDescriptorRanges = Size;
      result.DescriptorTable.pDescriptorRanges = Ranges.data();
      result.ShaderVisibility = D3D12_SHADER_VISIBILITY(Visibility);
      return result;
    }
  };
}