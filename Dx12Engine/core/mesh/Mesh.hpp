#pragma once

#include "../Types.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"

namespace DX12 {

    class Mesh {
    public:
        Mesh();
        ~Mesh();

        bool createTriangle(const GraphicsDevice& device);
        void draw(const GraphicsDevice& device) const;

        ComPtr<ID3D12Resource> getVertexBuffer() const { return m_vertexBuffer; }
        int getVertexCount() const { return m_vertexCount; }

    private:
        ComPtr<ID3D12Resource> m_vertexBuffer;
        ComPtr<ID3D12Resource> m_indexBuffer;

        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

        int m_vertexCount;
        int m_indexCount;
    };

}