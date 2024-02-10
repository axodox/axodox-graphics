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
    Pixel,
    Amplification,
    Mesh
  };

  struct AXODOX_GRAPHICS_API Shader
  {
    explicit Shader(std::vector<uint8_t>&& bytecode);
    explicit Shader(const std::filesystem::path& path);

    std::vector<uint8_t> Bytecode;

    explicit operator D3D12_SHADER_BYTECODE() const;

    virtual ShaderKind Type() const = 0;
    virtual ~Shader() = default;
  };

  struct AXODOX_GRAPHICS_API ComputeShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API VertexShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API DomainShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API HullShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API GeometryShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API PixelShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API AmplificationShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };

  struct AXODOX_GRAPHICS_API MeshShader : public Shader
  {
    using Shader::Shader;
    virtual ShaderKind Type() const override;
  };
}