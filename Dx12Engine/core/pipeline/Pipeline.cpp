#include "Pipeline.hpp"

#include <iostream>

#include "../d3dx12.h"

namespace DX12 {

    Pipeline::Pipeline() = default;
    Pipeline::~Pipeline() = default;

    bool Pipeline::initialize(GraphicsDevice& device, Shader& shader) {
        if (!createRootSignature(device)) return false;
        if (!createPipelineState(device, shader)) return false;
        return true;
    }

    bool Pipeline::createRootSignature(GraphicsDevice& device) {
        CD3DX12_ROOT_PARAMETER rootParameters[1];
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(_countof(rootParameters), rootParameters, 0, nullptr,
                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signatureBlob;
        ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                                 &signatureBlob, &errorBlob);

        if (FAILED(hr)) {
            if (errorBlob) {
                std::cerr << "[ERROR] Root signature serialization error: "
                          << static_cast<const char*>(errorBlob->GetBufferPointer()) << std::endl;
            }
            return false;
        }

        hr = device.getDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
                                                     signatureBlob->GetBufferSize(),
                                                     IID_PPV_ARGS(&m_rootSignature));

        if (FAILED(hr)) {
            std::cerr << "[ERROR] Failed to create root signature. HRESULT: 0x"
                      << std::hex << hr << std::dec << std::endl;
            return false;
        }

        std::cout << "[SUCCESS] Root signature created" << std::endl;
        return true;
    }

    bool Pipeline::createPipelineState(GraphicsDevice& device, Shader& shader) {
        if (!shader.getVertexBlob() || !shader.getPixelBlob()) {
            std::cerr << "[ERROR] Shaders not compiled!" << std::endl;
            return false;
        }

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
              D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
              D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28,
              D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = {
            shader.getVertexBlob()->GetBufferPointer(),
            shader.getVertexBlob()->GetBufferSize()
        };
        psoDesc.PS = {
            shader.getPixelBlob()->GetBufferPointer(),
            shader.getPixelBlob()->GetBufferSize()
        };

        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.DepthClipEnable = TRUE;

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;

        HRESULT hr = device.getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));

        if (FAILED(hr)) {
            std::cerr << "[ERROR] Failed to create pipeline state. HRESULT: 0x"
                      << std::hex << hr << std::dec << std::endl;

            if (hr == 0x80070057) {
                std::cerr << "[ERROR] E_INVALIDARG - Invalid argument. Possible causes:" << std::endl;
                std::cerr << "  - Mismatched vertex shader input layout" << std::endl;
                std::cerr << "  - Invalid render target format" << std::endl;
                std::cerr << "  - Root signature mismatch" << std::endl;

                std::cout << "[DEBUG] RTV Format: " << psoDesc.RTVFormats[0] << std::endl;
                std::cout << "[DEBUG] DSV Format: " << psoDesc.DSVFormat << std::endl;
                std::cout << "[DEBUG] Sample Count: " << psoDesc.SampleDesc.Count << std::endl;
            }

            return false;
        }

        std::cout << "[SUCCESS] Pipeline state created" << std::endl;
        return true;
    }

    void Pipeline::apply(const GraphicsDevice& device) const
    {
        const auto commandList = device.getCommandList();
        commandList->SetGraphicsRootSignature(m_rootSignature.Get());
        commandList->SetPipelineState(m_pipelineState.Get());
    }

}