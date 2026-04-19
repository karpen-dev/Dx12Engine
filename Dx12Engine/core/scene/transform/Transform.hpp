#pragma once

#include <DirectXMath.h>

namespace DX12 {

    class Transform {
    public:
        Transform();

        void setPosition(float x, float y, float z);
        void setPosition(const DirectX::XMFLOAT3& position);
        DirectX::XMFLOAT3 getPosition() const;

        void setRotation(float pitch, float yaw, float roll);
        void setRotation(const DirectX::XMFLOAT3& rotation);
        DirectX::XMFLOAT3 getRotation() const;

        void rotatePitch(float angle);
        void rotateYaw(float angle);
        void rotateRoll(float angle);

        void setScale(float x, float y, float z);
        void setScale(float uniform);
        DirectX::XMFLOAT3 getScale() const;

        DirectX::XMFLOAT3 getForward() const;
        DirectX::XMFLOAT3 getRight() const;
        DirectX::XMFLOAT3 getUp() const;

        DirectX::XMMATRIX getWorldMatrix() const;
        DirectX::XMFLOAT4X4 getWorldMatrixFloat() const;

        void translate(const DirectX::XMFLOAT3& delta);
        void translate(float x, float y, float z);

        void reset();

        DirectX::XMMATRIX getMatrix() const {
            return getWorldMatrix();
        }

        DirectX::XMFLOAT4X4 getMatrixFloat() const {
            return getWorldMatrixFloat();
        }

    private:
        void updateVectors();

        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_rotation;
        DirectX::XMFLOAT3 m_scale;

        DirectX::XMFLOAT3 m_forward;
        DirectX::XMFLOAT3 m_right;
        DirectX::XMFLOAT3 m_up;

        bool m_dirty;
    };

}