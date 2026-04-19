#include "Mesh.hpp"
#include <iostream>

#include "bufferManager/BufferManager.hpp"

namespace DX12 {
    bool Mesh::createFromData(const GraphicsDevice& device, const MeshData& meshData) {
        if (meshData.isEmpty()) {
            std::cerr << "[ERROR] MeshData is empty!" << std::endl;
            return false;
        }

        m_name = meshData.name;
        m_vertexCount = static_cast<int>(meshData.vertices.size());
        m_indexCount = static_cast<int>(meshData.indices.size());

        std::cout << "[DEBUG] Creating mesh '" << m_name << "'" << std::endl;
        std::cout << "  Vertices: " << m_vertexCount << std::endl;
        std::cout << "  Indices: " << m_indexCount << std::endl;

        if (m_useManager && m_bufferManager) {
            if (!m_bufferManager->createBuffer(device, meshData.vertices.data(),
                sizeof(Vertex) * meshData.vertices.size(), BufferType::Vertex, &m_vertexBufferId)) {
                std::cerr << "[ERROR] Failed to create vertex buffer via manager" << std::endl;
                return false;
            }

            if (!m_bufferManager->createBuffer(device, meshData.indices.data(),
                sizeof(uint32_t) * meshData.indices.size(), BufferType::Index, &m_indexBufferId)) {
                std::cerr << "[ERROR] Failed to create index buffer via manager" << std::endl;
                return false;
            }
        } else {
            m_vertexBuffer = std::make_unique<Buffer>();
            if (!m_vertexBuffer->create(device, meshData.vertices.data(),
                sizeof(Vertex) * meshData.vertices.size(), BufferType::Vertex)) {
                std::cerr << "[ERROR] Failed to create vertex buffer" << std::endl;
                return false;
            }

            m_indexBuffer = std::make_unique<Buffer>();
            if (!m_indexBuffer->create(device, meshData.indices.data(),
                sizeof(uint32_t) * meshData.indices.size(), BufferType::Index)) {
                std::cerr << "[ERROR] Failed to create index buffer" << std::endl;
                return false;
            }
        }

        std::cout << "[SUCCESS] Mesh '" << m_name << "' created successfully!" << std::endl;
        return true;
    }

    bool Mesh::createPyramid(const GraphicsDevice& device) {
        return createFromData(device, ModelLoader::createPyramid());
    }

    bool Mesh::createCube(const GraphicsDevice& device, float size) {
        return createFromData(device, ModelLoader::createCube(size));
    }

    bool Mesh::createSphere(const GraphicsDevice& device, float radius, int segments) {
        return createFromData(device, ModelLoader::createSphere(radius, segments));
    }

    bool Mesh::createPlane(const GraphicsDevice& device, float width, float depth) {
        return createFromData(device, ModelLoader::createPlane(width, depth));
    }

    void Mesh::draw(const GraphicsDevice& device) const {
        auto commandList = device.getCommandList();

        if (m_useManager && m_bufferManager) {
            m_bufferManager->bindVertexBuffer(device, m_vertexBufferId);
            m_bufferManager->bindIndexBuffer(device, m_indexBufferId);
        } else {
            if (m_vertexBuffer) m_vertexBuffer->bindVertex(device);
            if (m_indexBuffer) m_indexBuffer->bindIndex(device);
        }

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if (m_indexCount > 0) {
            commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
        } else {
            commandList->DrawInstanced(m_vertexCount, 1, 0, 0);
        }
    }

}