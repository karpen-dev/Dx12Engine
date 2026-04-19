#include "Transform.hpp"

namespace DX12
{
    Transform::Transform()
        : m_position(0.0f, 0.0f, 0.0f)
          , m_rotation(0.0f, 0.0f, 0.0f)
          , m_scale(1.0f, 1.0f, 1.0f)
          , m_matrixDirty(true)
    {
        DirectX::XMStoreFloat4x4(&m_cachedMatrix, DirectX::XMMatrixIdentity());
    }

    void Transform::setPosition(float x, float y, float z)
    {
        m_position = DirectX::XMFLOAT3(x, y, z);
        m_matrixDirty = true;
    }

    void Transform::setRotation(float pith, float yaw, float roll)
    {
        m_rotation = DirectX::XMFLOAT3(pith, yaw, roll);
        m_matrixDirty = true;
    }

    void Transform::setScale(float x, float y, float z)
    {
        m_scale = DirectX::XMFLOAT3(x, y, z);
        m_matrixDirty = true;
    }

    void Transform::translate(float x, float y, float z)
    {
        m_position.x += x;
        m_position.y += y;
        m_position.z += z;

        m_matrixDirty = true;
    }

    void Transform::rotate(float pith, float yaw, float roll)
    {
        m_rotation.x = pith;
        m_rotation.y = yaw;
        m_rotation.z = roll;

        m_matrixDirty = true;
    }

    DirectX::XMFLOAT4X4 Transform::getMatrix() const {
        if (m_matrixDirty) {
            DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
            DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

            DirectX::XMMATRIX result = scaleMatrix * rotationMatrix * translationMatrix;

            DirectX::XMStoreFloat4x4(&m_cachedMatrix, result);
            m_matrixDirty = false;
        }

        return m_cachedMatrix;
    }
}
