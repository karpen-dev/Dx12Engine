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

        bool initialize(int width, int height, const std::wstring& title);
        void run();
        void shutdown();

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