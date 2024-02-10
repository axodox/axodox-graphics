#include "pch.h"
#include "Shaders.h"

using namespace Axodox::Storage;
using namespace std;

namespace Axodox::Graphics::D3D12
{
  Shader::Shader(std::vector<uint8_t>&& bytecode) :
    Bytecode(move(bytecode))
  { }

  Shader::Shader(const std::filesystem::path& path) :
    Bytecode(try_read_file(path))
  { }

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

  ShaderKind AmplificationShader::Type() const
  {
    return ShaderKind::Amplification;
  }

  ShaderKind MeshShader::Type() const
  {
    return ShaderKind::Mesh;
  }
}
