#include "pch.h"
#include "Shaders.h"

namespace Axodox::Graphics::D3D12
{
  Shader::operator D3D12_SHADER_BYTECODE() const
  {
    return { Bytecode.data(), Bytecode.size() };
  }

  ShaderKind ComputeShader::Type() const
  {
    return ShaderKind::Compute;
  }

  ShaderKind VertexShader::Type() const
  {
    return ShaderKind::Vertex;
  }

  ShaderKind DomainShader::Type() const
  {
    return ShaderKind::Domain;
  }

  ShaderKind HullShader::Type() const
  {
    return ShaderKind::Hull;
  }

  ShaderKind GeometryShader::Type() const
  {
    return ShaderKind::Geometry;
  }

  ShaderKind PixelShader::Type() const
  {
    return ShaderKind::Pixel;
  }
}
