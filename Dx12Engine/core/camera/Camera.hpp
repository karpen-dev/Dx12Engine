#pragma once

#include "../Types.hpp"

namespace DX12 {

    class Camera {
    public:
        Camera();

        void update(float deltaTime);
        DirectX::XMFLOAT4X4 getMVPForObject(const DirectX::XMFLOAT4X4& worldMatrix) const;
        DirectX::XMFLOAT4X4 getModelViewMatrix() const;
        DirectX::XMFLOAT4X4 getViewProjectionMatrix() const;
        void setAspectRatio(float aspect);
        const DirectX::XMFLOAT4X4& getViewMatrix() const;
        const DirectX::XMFLOAT4X4& getProjectionMatrix() const;

        void setViewMatrix(const DirectX::XMFLOAT4X4& viewMatrix);

    private:
        DirectX::XMFLOAT4X4 m_viewMatrix;
        DirectX::XMFLOAT4X4 m_projectionMatrix;
        float m_rotationAngle;
        float m_aspectRatio;
    };

}