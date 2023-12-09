#pragma once
#include "pch.h"

namespace Axodox::Graphics::D3D12
{
  enum class ShaderKind
  {
    Compute,
    Vertex,
    Domain,
    Hull,
    Geometry,
    Pixel
  };

  struct Shader
  {
    explicit Shader(std::vector<uint8_t>&& bytecode);
    explicit Shader(const std::filesystem::path& path);

    std::vector<uint8_t> Bytecode;

    explicit operator D3D12_SHADER_BYTECODE() const;

    virtual ShaderKind Type() const = 0;
    virtual ~Shader() = default;
  };

  struct ComputeShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct VertexShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct DomainShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct HullShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct GeometryShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct PixelShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };
}