#pragma once

#include "../../mesh/Mesh.hpp"
#include "../transform/Transform.hpp"

namespace DX12
{
    class MeshRenderer
    {
    public:
        MeshRenderer() = default;
        ~MeshRenderer() = default;

        bool setMesh(const GraphicsDevice& device, std::unique_ptr<Mesh> mesh);
        bool createMesh(const GraphicsDevice& device, MeshType type, float parm1 = 0.0f, float param2 = 0.0f);

        void draw(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) const;

        Transform& getTransform() { return m_transform; }
        const Transform& getTransform() const { return m_transform; }

        Mesh* getMesh() const { return m_mesh.get(); }

    private:
        std::unique_ptr<Mesh> m_mesh;
        Transform m_transform;
    };
}