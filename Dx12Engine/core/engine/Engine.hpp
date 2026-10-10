#pragma once

#include <chrono>
#include <memory>

#include "../window/Window.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"
#include "../shader/Shader.hpp"
#include "../pipeline/Pipeline.hpp"
#include "../scene/Scene.hpp"
#include "../renderer/Renderer.hpp"
#include "../camera/Camera.hpp"

namespace DX12 {

    class Engine {
    public:
        Engine() = default;
        ~Engine() { shutdown(); }

        bool initialize(int width, int height, const std::wstring& title, std::unique_ptr<Scene> scene);
        bool initializeShaders(const std::string &vertexPath, const std::string& pixelPath);

        void run();
        void shutdown();

        Window& getWindow() noexcept { return *m_window; }
        const Window& getWindow() const noexcept { return *m_window; }

        Shader& getShader() noexcept { return *m_shader; }
        const Shader& getShader() const noexcept { return *m_shader; }

        Pipeline& getPipeline() noexcept { return *m_pipeline; }
        const Pipeline& getPipeline() const noexcept { return *m_pipeline; }

        Scene& getScene() noexcept { return *m_scene; }
        const Scene& getScene() const noexcept { return *m_scene; }

        Renderer& getRenderer() noexcept { return *m_renderer; }
        const Renderer& getRenderer() const noexcept { return *m_renderer; }

        GraphicsDevice& getGraphicsDevice() {
            return *m_graphicsDevice;
        }

        void setCamera(std::unique_ptr<Camera> camera) noexcept { m_camera = std::move(camera); }

        Camera& getCamera() noexcept { return *m_camera; }
        const Camera& getCamera() const noexcept { return *m_camera; }

    private:
        void update(float deltaTime);
        void render();

        std::unique_ptr<Window> m_window;
        std::unique_ptr<GraphicsDevice> m_graphicsDevice;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<Pipeline> m_pipeline;
        std::unique_ptr<DX12::Scene> m_scene;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<Camera> m_camera;

        std::chrono::steady_clock::time_point m_lastFrameTime;
        bool m_running;
    };

}