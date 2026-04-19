#pragma once

#include "../graphicsDevice/GraphicsDevice.hpp"
#include "../pipeline/Pipeline.hpp"
#include "../scene/Scene.hpp"
#include "../camera/Camera.hpp"

namespace DX12 {
    class Renderer {
    public:
        Renderer();
        ~Renderer();

        bool initialize(GraphicsDevice& device, Pipeline& pipeline);
        void shutdown();

        void setScene(Scene* scene);

        void render(GraphicsDevice& device, Camera& camera);

        void clear(GraphicsDevice& device, const float clearColor[4]);

    private:
        bool createConstantBuffer(GraphicsDevice& device);
        void updateConstantBuffer(GraphicsDevice& device,
                                 const DirectX::XMFLOAT4X4& mvp,
                                 const DirectX::XMFLOAT4X4& modelView,
                                 float time);

        Pipeline* m_pipeline;
        Scene* m_currentScene;

        ComPtr<ID3D12Resource> m_constantBuffer;
        void* m_constantBufferData;
        UINT m_constantBufferSize;

        bool m_initialized;
    };

}