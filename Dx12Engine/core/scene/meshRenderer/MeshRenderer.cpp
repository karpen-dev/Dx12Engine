#include "MeshRenderer.hpp"
#include "../../mesh/bufferManager/BufferManager.hpp"
#include "core/ConsoleLogger.hpp"

#include <iostream>

namespace DX12 {
    bool MeshRenderer::setMesh(const GraphicsDevice& device, std::unique_ptr<Mesh> mesh) {
        m_mesh = std::move(mesh);
        return true;
    }

    bool MeshRenderer::createMesh(const GraphicsDevice& device, MeshType type, float param1, float param2) {
        m_mesh = std::make_unique<Mesh>();

        switch (type) {
        case MeshType::Pyramid:
            return m_mesh->createPyramid(device);
        case MeshType::Cube:
            return m_mesh->createCube(device, param1);
        case MeshType::Sphere:
            return m_mesh->createSphere(device, param1, static_cast<int>(param2));
        case MeshType::Plane:
            return m_mesh->createPlane(device, param1, param2);
        default:
            ConsoleLogger::error("Unknown mesh type");
            return false;
        }
    }

    void MeshRenderer::draw(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) const
    {
        if (!m_mesh) return;

        m_mesh->draw(device);
    }

}
