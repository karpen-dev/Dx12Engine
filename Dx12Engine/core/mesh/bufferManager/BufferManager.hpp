#pragma once

#include <memory>
#include <unordered_map>

#include "../../Types.hpp"
#include "../../graphicsDevice/GraphicsDevice.hpp"

namespace DX12
{
    enum class BufferType
    {
        Vertex,
        Index,
        Constant
    };

    class Buffer
    {
    public:
        Buffer() = default;
        ~Buffer() = default;

        bool create(const GraphicsDevice& device,
                    const void* data,
                    UINT sizeInBytes,
                    BufferType type);

        void bindVertex(const GraphicsDevice& device, UINT slot = 0) const;
        void bindIndex(const GraphicsDevice& device) const;

        ID3D12Resource* getResource() const { return m_buffer.Get(); }
        UINT getSize() const { return m_sizeInBytes; }

    private:
        ComPtr<ID3D12Resource> m_buffer;

        D3D12_VERTEX_BUFFER_VIEW m_vertexView;
        D3D12_INDEX_BUFFER_VIEW m_indexView;

        BufferType m_type;
        UINT m_sizeInBytes;
    };

    class BufferManager
    {
    public:
        BufferManager() = default;
        ~BufferManager() = default;

        bool createBuffer(const GraphicsDevice& device,
                          const void* data,
                          UINT sizeInBytes,
                          BufferType type,
                          UINT* outBufferId);

        const Buffer* getBuffer(UINT id) const;

        void bindVertexBuffer(const GraphicsDevice& device, UINT id, UINT slot = 0) const;
        void bindIndexBuffer(const GraphicsDevice& device, UINT id) const;

        void clear();

    private:
        std::unordered_map<UINT, std::unique_ptr<Buffer>> m_buffers;
        UINT m_nextId = 0;
    };
}