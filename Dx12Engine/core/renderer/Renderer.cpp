#include "Renderer.hpp"

#include <iostream>

#include "../d3dx12.h"

namespace DX12 {

    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    bool Renderer::initialize(GraphicsDevice& device, Pipeline& pipeline, Mesh& mesh) {
        m_pipeline = &pipeline;
        m_mesh = &mesh;
        return createConstantBuffer(device);
    }

    bool Renderer::createConstantBuffer(GraphicsDevice& device) {
        m_constantBufferSize = (sizeof(TransformCB) + 255) & ~255;

        const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_constantBufferSize);

        const HRESULT hr = device.getDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_constantBuffer)
        );

        if (FAILED(hr)) return false;

        const CD3DX12_RANGE readRange(0, 0);
        m_constantBuffer->Map(0, &readRange, &m_constantBufferData);

        return true;
    }

    void Renderer::updateConstantBuffer(GraphicsDevice& device,
                                     const DirectX::XMFLOAT4X4& mvp,
                                     const DirectX::XMFLOAT4X4& modelView,
                                     const float time) {
        TransformCB cb;
        cb.modelViewProjection = mvp;
        cb.modelView = modelView;
        cb.lightDirection = DirectX::XMFLOAT3(0.5f, -1.0f, 0.3f);
        cb.time = time;

        static int frame = 0;
        if (frame++ < 5) {
            std::cout << "[DEBUG] MVP matrix:" << std::endl;
            std::cout << "  " << mvp._11 << ", " << mvp._12 << ", " << mvp._13 << ", " << mvp._14 << std::endl;
            std::cout << "  " << mvp._21 << ", " << mvp._22 << ", " << mvp._23 << ", " << mvp._24 << std::endl;
            std::cout << "  " << mvp._31 << ", " << mvp._32 << ", " << mvp._33 << ", " << mvp._34 << std::endl;
            std::cout << "  " << mvp._41 << ", " << mvp._42 << ", " << mvp._43 << ", " << mvp._44 << std::endl;

            std::cout << "[DEBUG] ModelView matrix:" << std::endl;
            std::cout << "  " << modelView._11 << ", " << modelView._12 << ", " << modelView._13 << ", " << modelView._14 << std::endl;
            std::cout << "  " << modelView._21 << ", " << modelView._22 << ", " << modelView._23 << ", " << modelView._24 << std::endl;
            std::cout << "  " << modelView._31 << ", " << modelView._32 << ", " << modelView._33 << ", " << modelView._34 << std::endl;
            std::cout << "  " << modelView._41 << ", " << modelView._42 << ", " << modelView._43 << ", " << modelView._44 << std::endl;
        }

        memcpy(m_constantBufferData, &cb, sizeof(cb));
    }

    void Renderer::render(GraphicsDevice& device, Camera& camera) {
        const auto commandList = device.getCommandList();

        const DirectX::XMFLOAT4X4 mvp = camera.getViewProjectionMatrix();
        const DirectX::XMFLOAT4X4 modelView = camera.getModelViewMatrix();
        const float time = static_cast<float>(GetTickCount64()) / 1000.0f;

        updateConstantBuffer(device, mvp, modelView, time);

        commandList->SetPipelineState(m_pipeline->getPipelineState().Get());
        commandList->SetGraphicsRootSignature(m_pipeline->getRootSignature().Get());

        const D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_constantBuffer->GetGPUVirtualAddress();
        commandList->SetGraphicsRootConstantBufferView(0, cbAddress);

        m_mesh->draw(device);
    }

}