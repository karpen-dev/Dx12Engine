#pragma once

#include "../../mesh/Mesh.hpp"
#include "../transform/Transform.hpp"
#include "../SceneObject.hpp"

namespace DX12
{
    class MeshRenderer : public SceneObject
    {
    public:
        MeshRenderer() = default;
        ~MeshRenderer() override = default;

        bool setMesh(const GraphicsDevice& device, std::unique_ptr<Mesh> mesh);
        bool createMesh(const GraphicsDevice& device, MeshType type, float parm1 = 0.0f, float param2 = 0.0f);

        void draw(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) override;

        Mesh* getMesh() const { return m_mesh.get(); }

        void update(float deltaTime) override;
    private:
        std::unique_ptr<Mesh> m_mesh;
    };
}