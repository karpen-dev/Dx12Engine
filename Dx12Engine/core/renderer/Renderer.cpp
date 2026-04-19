#include "Renderer.hpp"
#include <iostream>
#include "../d3dx12.h"
#include "../../core/ConsoleLogger.hpp"

namespace DX12 {

    Renderer::Renderer()
        : m_pipeline(nullptr)
        , m_currentScene(nullptr)
        , m_constantBufferData(nullptr)
        , m_constantBufferSize(0)
        , m_initialized(false) {
    }

    Renderer::~Renderer() {
        shutdown();
    }

    bool Renderer::initialize(GraphicsDevice& device, Pipeline& pipeline) {
        ConsoleLogger::info("Initializing renderer...");

        m_pipeline = &pipeline;

        if (!createConstantBuffer(device)) {
            ConsoleLogger::error("Failed to create constant buffer");
            return false;
        }

        m_initialized = true;
        ConsoleLogger::success("Renderer initialized successfully");
        return true;
    }

    void Renderer::shutdown() {
        if (!m_initialized) return;

        ConsoleLogger::info("Shutting down renderer...");

        if (m_constantBuffer && m_constantBufferData) {
            m_constantBuffer->Unmap(0, nullptr);
            m_constantBufferData = nullptr;
        }

        m_constantBuffer.Reset();
        m_pipeline = nullptr;
        m_currentScene = nullptr;
        m_initialized = false;

        ConsoleLogger::success("Renderer shutdown complete");
    }

    bool Renderer::createConstantBuffer(GraphicsDevice& device) {
        m_constantBufferSize = (sizeof(TransformCB) + 255) & ~255;

        m_constantBuffer = device.createUploadBuffer(m_constantBufferSize);

        if (!m_constantBuffer) {
            ConsoleLogger::error("Failed to create constant buffer");
            return false;
        }

        CD3DX12_RANGE readRange(0, 0);
        HRESULT hr = m_constantBuffer->Map(0, &readRange, &m_constantBufferData);

        if (FAILED(hr)) {
            ConsoleLogger::error("Failed to map constant buffer");
            return false;
        }

        ConsoleLogger::success("Constant buffer created successfully");
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

    void Renderer::setScene(Scene* scene) {
        m_currentScene = scene;
        if (scene) {
            ConsoleLogger::info("Renderer: Scene set with " +
                               std::to_string(scene->getObjectCount()) + " objects");
        } else {
            ConsoleLogger::info("Renderer: Scene cleared");
        }
    }

    void Renderer::clear(GraphicsDevice& device, const float clearColor[4]) {
        auto commandList = device.getCommandList();

        D3D12_CPU_DESCRIPTOR_HANDLE rtv = device.getCurrentRTV();
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = device.getDSV();

        commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

        commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    }

    void Renderer::render(GraphicsDevice& device, Camera& camera) {
        if (!m_initialized || !m_pipeline || !m_currentScene) {
            ConsoleLogger::error("Renderer not properly initialized or no scene set!");
            return;
        }

        auto commandList = device.getCommandList();

        commandList->SetPipelineState(m_pipeline->getPipelineState().Get());
        commandList->SetGraphicsRootSignature(m_pipeline->getRootSignature().Get());

        const float time = static_cast<float>(GetTickCount64()) / 1000.0f;

        DirectX::XMFLOAT4X4 viewMatrix = camera.getModelViewMatrix();
        DirectX::XMFLOAT4X4 projectionMatrix = camera.getViewProjectionMatrix();

        DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&viewMatrix);
        DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&projectionMatrix);

        const auto& objects = m_currentScene->getAllObjects();
        for (const auto& renderer : objects) {
            DirectX::XMFLOAT4X4 worldMatrix4x4 = renderer->getTransform().getMatrix();
            DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&worldMatrix4x4);

            DirectX::XMMATRIX mvpMat = worldMatrix * viewMat * projMat;
            DirectX::XMMATRIX modelViewMat = worldMatrix * viewMat;

            DirectX::XMFLOAT4X4 mvp, modelView;
            DirectX::XMStoreFloat4x4(&mvp, mvpMat);
            DirectX::XMStoreFloat4x4(&modelView, modelViewMat);

            updateConstantBuffer(device, mvp, modelView, time);

            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_constantBuffer->GetGPUVirtualAddress();
            commandList->SetGraphicsRootConstantBufferView(0, cbAddress);

            if (renderer->getMesh()) {
                renderer->getMesh()->draw(device);
            }
        }
    }

}