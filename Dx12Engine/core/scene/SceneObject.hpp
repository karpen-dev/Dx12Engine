#pragma once

#include <string>

#include <DirectXMath.h>

#include "transform/Transform.hpp"

namespace DX12 {

    class GraphicsDevice;

    class SceneObject {
    public:
        virtual ~SceneObject() = default;

        virtual void onAdd() {}
        virtual void onRemove() {}
        virtual void update(float deltaTime) {}

        virtual void render(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) {}
        virtual void draw(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) {}

        const std::string& getName() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        Transform& getTransform() { return m_transform; }
        const Transform& getTransform() const { return m_transform; }

        void setTransform(Transform transform) { m_transform = std::move(transform); }

        bool isActive() const { return m_active; }
        void setActive(bool active) { m_active = active; }

    protected:
        std::string m_name;
        Transform m_transform;
        bool m_active = true;
    };

}
