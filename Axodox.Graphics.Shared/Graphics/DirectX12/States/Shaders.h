#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  enum class ShaderKind
  {
    Vertex,
    Pixel,
    Domain,
    Hull,
    Geometry
  };

  struct Shader
  {
    std::vector<std::uint8_t> Bytecode;
        
    explicit operator D3D12_SHADER_BYTECODE() const;

    virtual ShaderKind Type() const = 0;
    virtual ~Shader() = default;
  };

  struct VertexShader : public Shader
  {
    virtual ShaderKind Type() const override;
  };

  struct PixelShader : public Shader
  {
    virtual ShaderKind Type() const override;
  };

  struct DomainShader : public Shader
  {
    virtual ShaderKind Type() const override;
  };
  
  struct HullShader : public Shader
  {
    virtual ShaderKind Type() const override;
  };

  struct GeometryShader : public Shader
  {
    virtual ShaderKind Type() const override;
  };
}