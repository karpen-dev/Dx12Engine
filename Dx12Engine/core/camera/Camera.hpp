#pragma once

#include "../Types.hpp"

namespace DX12 {

    class Camera {
    public:
        Camera();

        void update(float deltaTime);
        DirectX::XMFLOAT4X4 getModelViewMatrix() const;
        DirectX::XMFLOAT4X4 getViewProjectionMatrix() const;
        void setAspectRatio(float aspect);

    private:
        DirectX::XMFLOAT4X4 m_viewMatrix;
        DirectX::XMFLOAT4X4 m_projectionMatrix;
        float m_rotationAngle;
        float m_aspectRatio;
    };

}