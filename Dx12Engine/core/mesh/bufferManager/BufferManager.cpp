#include "BufferManager.hpp"
#include "../../d3dx12.h"

namespace DX12
{
    bool Buffer::create(const GraphicsDevice& device, const void* data, UINT sizeInBytes, BufferType type)
    {
        m_type = type;
        m_sizeInBytes = sizeInBytes;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);

        HRESULT hr = device.getDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer)
        );

        if (FAILED(hr))
            return false;

        void* mappedData;
        const CD3DX12_RANGE readRange(0, 0);
        hr = m_buffer->Map(0, &readRange, &mappedData);

        if (FAILED(hr))
            return false;

        memcpy(mappedData, data, sizeInBytes);
        m_buffer->Unmap(0, nullptr);

        if (type == BufferType::Vertex)
        {
            m_vertexView.BufferLocation = m_buffer->GetGPUVirtualAddress();
            m_vertexView.StrideInBytes = sizeof(Vertex);
            m_vertexView.SizeInBytes = sizeInBytes;
        } else if (type == BufferType::Index)
        {
            m_indexView.BufferLocation = m_buffer->GetGPUVirtualAddress();
            m_indexView.Format = DXGI_FORMAT_R32_UINT;
            m_indexView.SizeInBytes = sizeInBytes;
        }

        return true;
    }

    void Buffer::bindVertex(const GraphicsDevice& device, UINT slot) const
    {
        if (m_type == BufferType::Vertex)
        {
            device.getCommandList()->IASetVertexBuffers(slot, 1, &m_vertexView);
        }
    }

    void Buffer::bindIndex(const GraphicsDevice& device) const
    {
        if (m_type == BufferType::Index)
        {
            device.getCommandList()->IASetIndexBuffer(&m_indexView);
        }
    }

    bool BufferManager::createBuffer(const GraphicsDevice& device, const void* data, UINT sizeInBytes, BufferType type, UINT* outBufferId)
    {
        auto buffer = std::make_unique<Buffer>();
        if (!buffer->create(device, data, sizeInBytes, type))
        {
            return false;
        }

        const UINT id = m_nextId++;
        m_buffers[id] = std::move(buffer);

        if (outBufferId)
            *outBufferId = id;

        return true;
    }

    const Buffer* BufferManager::getBuffer(const UINT id) const {
        const auto it = m_buffers.find(id);
        return it != m_buffers.end() ? it->second.get() : nullptr;
    }

    void BufferManager::bindVertexBuffer(const GraphicsDevice& device, UINT id, UINT slot) const
    {
        const auto* buffer = getBuffer(id);
        if (buffer)
            buffer->bindVertex(device, slot);
    }

    void BufferManager::bindIndexBuffer(const GraphicsDevice& device, UINT id) const
    {
        const auto* buffer = getBuffer(id);
        if (buffer)
            buffer->bindIndex(device);
    }

    void BufferManager::clear()
    {
        m_buffers.clear();
        m_nextId = 0;
    }
}