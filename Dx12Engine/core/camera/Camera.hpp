#pragma once

#include "../Types.hpp"

namespace DX12 {

    class Camera {
    public:
        Camera();

        void update(float deltaTime);

        template <typename T>
        T clamp(const T& v, const T& lo, const T& hi) {
            return (v < lo) ? lo : (hi < v) ? hi : v;
        }

        void addYaw  (float d) { m_yaw   += d; m_dirty = true; }
        void addPitch(float d) { m_pitch = clamp(m_pitch + d, -80.0f, 80.0f); m_dirty = true; }

        float getYaw() const { return m_yaw; }

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

        const DirectX::XMFLOAT4X4& getProjectionMatrix() const;

        DirectX::XMMATRIX getViewMatrix() const {
            float y = DirectX::XMConvertToRadians(m_yaw);
            float p = DirectX::XMConvertToRadians(m_pitch);
            DirectX::XMVECTOR offset = DirectX::XMVectorSet(
                m_distance * cosf(p) * sinf(y),
                m_distance * sinf(p),
                m_distance * cosf(p) * cosf(y),
                0);
            DirectX::XMVECTOR target = DirectX::XMLoadFloat3(&m_target);
            DirectX::XMVECTOR eye    = DirectX::XMVectorAdd(target, offset);
            return DirectX::XMMatrixLookAtLH(eye, target, DirectX::XMVectorSet(0,1,0,0));
        }

    private:
        void updateViewMatrix();

        DirectX::XMFLOAT4X4 m_viewMatrix;
        DirectX::XMFLOAT4X4 m_projectionMatrix;

        float m_rotationAngle;
        float m_aspectRatio;
        float m_distance;

        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_target;

        float m_yaw = 0, m_pitch = 15.0f;
        float speed = 5.0f;
        bool  m_dirty = true;
    };

}