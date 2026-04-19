#pragma once

#include <memory>

#include "../Types.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"
#include "./bufferManager/BufferManager.hpp"
#include "./modelLoader/ModelLoader.hpp"

namespace DX12 {
    class Buffer;
    class BufferManager;

    class Mesh {
    public:
        Mesh() = default;
        ~Mesh() = default;

        bool createFromData(const GraphicsDevice& device,
                            const MeshData& meshData);

        bool createPyramid(const GraphicsDevice& device);
        bool createCube(const GraphicsDevice& device, float size = 1.0f);
        bool createSphere(const GraphicsDevice& device, float radius = 0.5f, int segments = 16);
        bool createPlane(const GraphicsDevice& device, float width = 2.0f, float depth = 2.0f);

        void draw(const GraphicsDevice& device) const;

        const std::string& getName() const { return m_name; }
        int getVertexCount() const { return m_vertexCount; }
        int getIndexCount() const { return m_indexCount; }

        void setBufferManager(BufferManager* manager) { m_bufferManager = manager; }

    private:
        BufferManager* m_bufferManager;

        std::unique_ptr<Buffer> m_vertexBuffer;
        std::unique_ptr<Buffer> m_indexBuffer;

        UINT m_vertexBufferId;
        UINT m_indexBufferId;

        std::string m_name;
        int m_vertexCount;
        int m_indexCount;

        bool m_useManager;
    };
}