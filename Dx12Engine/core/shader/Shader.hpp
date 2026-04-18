#pragma once

#include "../Types.hpp"
#include <string>

namespace DX12
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        bool compileVertexShader(const std::string& filepath, const std::string& entryPoint = "main");
        bool compilePixelShader(const std::string& filepath, const std::string& entryPoint = "main");

        ComPtr<ID3DBlob> getVertexBlob() const { return m_vertexBlob; }
        ComPtr<ID3DBlob> getPixelBlob() const { return m_pixelBlob; }

    private:
        bool compileShader(const std::string& filepath, const std::string& entryPoint,
                           const std::string& target, ComPtr<ID3DBlob>& blob);

        ComPtr<ID3DBlob> m_vertexBlob;
        ComPtr<ID3DBlob> m_pixelBlob;
    };
}