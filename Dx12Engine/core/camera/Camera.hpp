#pragma once

#include "../Types.hpp"

namespace DX12 {

    class Camera {
    public:
        Camera();

        void update(float deltaTime);

        void setPosition(const DirectX::XMFLOAT3& position);
        void setPosition(float x, float y, float z);
        void setTarget(const DirectX::XMFLOAT3& target);
        void setTarget(float x, float y, float z);
        void setRotation(float angle);
        void setDistance(float distance);

        DirectX::XMFLOAT3 getPosition() const { return m_position; }
        DirectX::XMFLOAT3 getTarget() const { return m_target; }
        float getRotation() const { return m_rotationAngle; }
        float getDistance() const { return m_distance; }

        DirectX::XMFLOAT4X4 getMVPForObject(const DirectX::XMFLOAT4X4& worldMatrix) const;
        DirectX::XMFLOAT4X4 getViewProjectionMatrix() const;
        DirectX::XMFLOAT4X4 getModelViewMatrix() const;

        void setAspectRatio(float aspect);

        const DirectX::XMFLOAT4X4& getViewMatrix() const;
        const DirectX::XMFLOAT4X4& getProjectionMatrix() const;

    private:
        void updateViewMatrix();

        DirectX::XMFLOAT4X4 m_viewMatrix;
        DirectX::XMFLOAT4X4 m_projectionMatrix;

        float m_rotationAngle;
        float m_aspectRatio;
        float m_distance;

        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_target;
    };

}