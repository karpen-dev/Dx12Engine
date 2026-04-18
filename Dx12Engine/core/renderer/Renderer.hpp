#pragma once

#include "../Types.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"
#include "../pipeline/Pipeline.hpp"
#include "../mesh/Mesh.hpp"
#include "../camera/Camera.hpp"

namespace DX12 {

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        bool initialize(GraphicsDevice& device, Pipeline& pipeline, Mesh& mesh);
        void render(GraphicsDevice& device, Camera& camera);

    private:
        bool createConstantBuffer(GraphicsDevice& device);
        void updateConstantBuffer(GraphicsDevice& device, const DirectX::XMFLOAT4X4& mvp, const DirectX::XMFLOAT4X4& modelView, float time);

        ComPtr<ID3D12Resource> m_constantBuffer;
        void* m_constantBufferData;
        UINT m_constantBufferSize;
        Pipeline* m_pipeline;
        Mesh* m_mesh;
    };

}