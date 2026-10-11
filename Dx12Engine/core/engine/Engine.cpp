#include "Engine.hpp"
#include "../d3dx12.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "../ConsoleLogger.hpp"
#include <iostream>
#include <cassert>

namespace DX12 {

    bool Engine::initializeShaders(const std::string &vertexPath, const std::string& pixelPath) {
        ConsoleLogger::info("Initializing shaders...");

        m_shader = std::make_unique<Shader>();

        ConsoleLogger::debug(std::string("Looking for vertex shader at: ") + vertexPath);
        if (!m_shader->compileVertexShader(vertexPath)) {
            ConsoleLogger::error("Failed to compile vertex shader");
            return false;
        }

        ConsoleLogger::debug(std::string("Looking for pixel shader at: ") + pixelPath);
        if (!m_shader->compilePixelShader(pixelPath)) {
            ConsoleLogger::error("Failed to compile pixel shader");
            return false;
        }

        ConsoleLogger::success("Shaders compiled successfully");
        return true;
    }

    bool Engine::initialize(int width, int height, const std::wstring& title, std::unique_ptr<Scene> scene) {
        ConsoleLogger::info("Initializing engine...");

        ConsoleLogger::info("Creating window...");
        m_window = std::make_unique<Window>(width, height, title);
        if (!m_window->init()) {
            ConsoleLogger::error("Failed to initialize window");
            return false;
        }
        ConsoleLogger::success("Window created successfully");

        ConsoleLogger::info("Creating graphics device...");
        m_graphicsDevice = std::make_unique<GraphicsDevice>();
        if (!m_graphicsDevice->initialize(*m_window)) {
            ConsoleLogger::error("Failed to initialize graphics device");
            return false;
        }
        ConsoleLogger::success("Graphics device created successfully");

        ConsoleLogger::info("Creating pipeline...");
        m_pipeline = std::make_unique<Pipeline>();
        if (!m_pipeline->initialize(*m_graphicsDevice, *m_shader)) {
            ConsoleLogger::error("Failed to initialize pipeline");
            return false;
        }
        ConsoleLogger::success("Pipeline created successfully");

        ConsoleLogger::info("Loading scene...");
        m_scene = std::move(scene);
        m_scene->load();

        ConsoleLogger::info("Creating renderer...");
        m_renderer = std::make_unique<Renderer>();
        if (!m_renderer->initialize(*m_graphicsDevice, *m_pipeline)) {
            ConsoleLogger::error("Failed to initialize renderer");
            return false;
        }

        m_renderer->setScene(m_scene.get());
        ConsoleLogger::success("Renderer created successfully");

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = m_graphicsDevice->getDevice();
        init_info.CommandQueue = m_graphicsDevice->getCommandQueue().Get();
        init_info.NumFramesInFlight = 2;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

        init_info.SrvDescriptorHeap = m_graphicsDevice->getSRVHeap();

        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
            *out_cpu_handle = info->SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            *out_gpu_handle = info->SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        };init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
            *out_cpu_handle = info->SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            *out_gpu_handle = info->SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        };

        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
            (void)info; (void)cpu_handle; (void)gpu_handle;
        };

        ImGui_ImplDX12_Init(&init_info);

        m_fpsHistory.resize(m_maxHistorySize, 0.0f);

        m_lastFrameTime = std::chrono::steady_clock::now();
        m_running = true;

        ConsoleLogger::success("Engine initialized successfully!");
        return true;
    }

    void Engine::run() {
        ConsoleLogger::info("\n=== Starting Main Loop ===");
        ConsoleLogger::info("  - Triangle is rotating around Y axis");
        ConsoleLogger::info("  - Press ESC to exit");
        ConsoleLogger::info("  - Close window to exit");
        std::cout << "\n";

        while (m_running && m_window->proccessMessages()) {
            auto now = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(now - m_lastFrameTime).count();

            if (deltaTime > 0.1f) deltaTime = 0.1f;

            m_lastFrameTime = now;

            update(deltaTime);
            render();

            Sleep(1);
        }
    }

    void Engine::updateUI() {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (m_uiCallback) {
            m_uiCallback();
        }

        ImGui::Render();
    }

    void Engine::renderUI(ID3D12GraphicsCommandList* commandList) {
        ID3D12DescriptorHeap* heaps[] = { m_graphicsDevice->getSRVHeap() };
        commandList->SetDescriptorHeaps(_countof(heaps), heaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
    }

    void Engine::update(float deltaTime) {
        m_camera->update(deltaTime);
        m_scene->update(deltaTime);
        m_window->endFrame();

        updateUI();

        m_fpsTimer += deltaTime;
        m_frameCount++;

        if (m_fpsTimer >= 1.0f) {
            m_currentFPS = static_cast<float>(m_frameCount) / m_fpsTimer;
            m_frameCount = 0;
            m_fpsTimer = 0.0f;
        }

        if (deltaTime <= 0.0f) deltaTime = 0.001f;
        float currentInstantFPS = 1.0f / deltaTime;

        for (size_t i = 0; i < m_fpsHistory.size() - 1; ++i) {
            m_fpsHistory[i] = m_fpsHistory[i + 1];
        }

        m_fpsHistory.back() = currentInstantFPS;

        auto& objects = m_scene->getAllObjects();
        for (auto& obj : objects) {
            obj->update(deltaTime);
        }
    }

    void Engine::render() {
        if (!m_graphicsDevice || !m_renderer || !m_camera) return;

        m_graphicsDevice->beginFrame();

        ID3D12GraphicsCommandList* currentCommandList = m_graphicsDevice->getCommandList();

        m_renderer->render(*m_graphicsDevice, *m_camera);

        renderUI(currentCommandList);

        m_graphicsDevice->endFrame();
        m_graphicsDevice->present(true);
    }

    void Engine::shutdown() {
        if (m_graphicsDevice) {
            m_graphicsDevice->waitForGPU();

            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            m_graphicsDevice->shutdown();
        }

        m_running = false;

        std::cout << "\n";
        ConsoleLogger::info("=== Engine Shutdown ===");
        ConsoleLogger::success("Engine shut down successfully!\n");
    }

}