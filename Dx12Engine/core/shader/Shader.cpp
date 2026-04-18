#include "Shader.hpp"

#include<d3dcompiler.h>

#include "../ConsoleLogger.hpp"

#pragma comment(lib, "d3dcompiler.lib")

namespace DX12
{
    Shader::Shader() = default;
    Shader::~Shader() = default;

    bool Shader::compileVertexShader(const std::string& filepath, const std::string& entryPoint)
    {
        return compileShader(filepath, entryPoint, "vs_5_0", m_vertexBlob);
    }

    bool Shader::compilePixelShader(const std::string& filepath, const std::string& entryPoint)
    {
        return compileShader(filepath, entryPoint, "ps_5_0", m_pixelBlob);
    }

    bool Shader::compileShader(const std::string& filepath, const std::string& entryPoint, const std::string& target, ComPtr<ID3DBlob>& blob)
    {
        ComPtr<ID3DBlob> errorBlob;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        const HRESULT hr = D3DCompileFromFile(
            std::wstring(filepath.begin(), filepath.end()).c_str(),
            nullptr, nullptr,
            entryPoint.c_str(),
            target.c_str(),
            flags, 0,
            &blob, &errorBlob
        );

        ConsoleLogger::success("Shader compiled, size: " + std::to_string(blob->GetBufferSize()));

        if (FAILED(hr))
        {
            if (errorBlob)
            {
                OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
            }

            return false;
        }

        return true;

    }

}