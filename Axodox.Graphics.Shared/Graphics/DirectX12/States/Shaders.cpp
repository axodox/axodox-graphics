#include "Shaders.h"

namespace Axodox::Graphics::D3D12
{
  Shader::operator D3D12_SHADER_BYTECODE() const
  {
    return { Bytecode.data(), Bytecode.size() };
  }

  ShaderKind VertexShader::Type() const
  {
    return ShaderKind::Vertex;
  }

  ShaderKind PixelShader::Type() const
  {
    return ShaderKind::Pixel;
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
}
