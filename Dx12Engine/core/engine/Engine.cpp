#include "Engine.hpp"
#include "../d3dx12.h"

#include <chrono>
#include <cassert>

#include "../ConsoleLogger.hpp"

namespace DX12 {

    Engine::Engine()
        : m_running(false) {
    }

    Engine::~Engine() {
        shutdown();
    }

    bool Engine::initialize(int width, int height, const std::wstring& title) {
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

        ConsoleLogger::info("Compiling shaders...");
        m_shader = std::make_unique<Shader>();

        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        ConsoleLogger::debug(std::string("Current directory: ") + buffer);

        std::string shaderPath = std::string(buffer) + "\\shaders\\VertexShader.hlsl";
        ConsoleLogger::debug(std::string("Looking for vertex shader at: ") + shaderPath);

        if (!m_shader->compileVertexShader(shaderPath)) {
            ConsoleLogger::error("Failed to compile vertex shader");
            ConsoleLogger::info("Make sure shaders directory exists and contains VertexShader.hlsl");
            return false;
        }

        shaderPath = std::string(buffer) + "\\shaders\\PixelShader.hlsl";
        ConsoleLogger::debug(std::string("Looking for pixel shader at: ") + shaderPath);

        if (!m_shader->compilePixelShader(shaderPath)) {
            ConsoleLogger::error("Failed to compile pixel shader");
            return false;
        }
        ConsoleLogger::success("Shaders compiled successfully");

        ConsoleLogger::info("Creating pipeline...");
        m_pipeline = std::make_unique<Pipeline>();
        if (!m_pipeline->initialize(*m_graphicsDevice, *m_shader)) {
            ConsoleLogger::error("Failed to initialize pipeline");
            return false;
        }
        ConsoleLogger::success("Pipeline created successfully");

        ConsoleLogger::info("Creating triangle mesh...");
        m_mesh = std::make_unique<Mesh>();
        if (!m_mesh->createTriangle(*m_graphicsDevice)) {
            ConsoleLogger::error("Failed to create triangle mesh");
            return false;
        }
        ConsoleLogger::success("Triangle mesh created successfully");

        ConsoleLogger::info("Creating renderer...");
        m_renderer = std::make_unique<Renderer>();
        if (!m_renderer->initialize(*m_graphicsDevice, *m_pipeline, *m_mesh)) {
            ConsoleLogger::error("Failed to initialize renderer");
            return false;
        }
        ConsoleLogger::success("Renderer created successfully");

        ConsoleLogger::info("Creating camera...");
        m_camera = std::make_unique<Camera>();
        m_camera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
        ConsoleLogger::success("Camera created successfully");

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

    void Engine::update(float deltaTime) {
        m_camera->update(deltaTime);
    }

    void Engine::render() {
        if (!m_graphicsDevice || !m_renderer || !m_camera) return;

        m_graphicsDevice->beginFrame();

        m_renderer->render(*m_graphicsDevice, *m_camera);

        m_graphicsDevice->endFrame();
        m_graphicsDevice->present(true);
    }

    void Engine::shutdown() {
        if (m_graphicsDevice) {
            m_graphicsDevice->waitForGPU();
            m_graphicsDevice->shutdown();
        }

        m_running = false;

        std::cout << "\n";
        ConsoleLogger::info("=== Engine Shutdown ===");
        ConsoleLogger::success("Engine shut down successfully!\n");
    }

}