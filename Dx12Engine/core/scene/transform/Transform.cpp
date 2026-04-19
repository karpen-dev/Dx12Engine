#include "Transform.hpp"

#include <cmath>

namespace DX12 {

    Transform::Transform() 
        : m_position(0.0f, 0.0f, 0.0f)
        , m_rotation(0.0f, 0.0f, 0.0f)
        , m_scale(1.0f, 1.0f, 1.0f)
        , m_forward(0.0f, 0.0f, 1.0f)
        , m_right(1.0f, 0.0f, 0.0f)
        , m_up(0.0f, 1.0f, 0.0f)
        , m_dirty(true) {
    }

    void Transform::setPosition(float x, float y, float z) {
        m_position = DirectX::XMFLOAT3(x, y, z);
    }

    void Transform::setPosition(const DirectX::XMFLOAT3& position) {
        m_position = position;
    }

    DirectX::XMFLOAT3 Transform::getPosition() const {
        return m_position;
    }

    void Transform::setRotation(float pitch, float yaw, float roll) {
        m_rotation = DirectX::XMFLOAT3(pitch, yaw, roll);
        m_dirty = true;
    }

    void Transform::setRotation(const DirectX::XMFLOAT3& rotation) {
        m_rotation = rotation;
        m_dirty = true;
    }

    DirectX::XMFLOAT3 Transform::getRotation() const {
        return m_rotation;
    }

    void Transform::rotatePitch(float angle) {
        m_rotation.x += angle;

        if (m_rotation.x > DirectX::XM_PIDIV2) m_rotation.x = DirectX::XM_PIDIV2;
        if (m_rotation.x < -DirectX::XM_PIDIV2) m_rotation.x = -DirectX::XM_PIDIV2;
        m_dirty = true;
    }

    void Transform::rotateYaw(float angle) {
        m_rotation.y += angle;

        if (m_rotation.y > DirectX::XM_2PI) m_rotation.y -= DirectX::XM_2PI;
        if (m_rotation.y < 0) m_rotation.y += DirectX::XM_2PI;
        m_dirty = true;
    }

    void Transform::rotateRoll(float angle) {
        m_rotation.z += angle;
        m_dirty = true;
    }

    void Transform::setScale(float x, float y, float z) {
        m_scale = DirectX::XMFLOAT3(x, y, z);
    }

    void Transform::setScale(float uniform) {
        m_scale = DirectX::XMFLOAT3(uniform, uniform, uniform);
    }

    DirectX::XMFLOAT3 Transform::getScale() const {
        return m_scale;
    }

    void Transform::translate(const DirectX::XMFLOAT3& delta) {
        m_position.x += delta.x;
        m_position.y += delta.y;
        m_position.z += delta.z;
    }

    void Transform::translate(float x, float y, float z) {
        m_position.x += x;
        m_position.y += y;
        m_position.z += z;
    }

    DirectX::XMFLOAT3 Transform::getForward() const {
        if (m_dirty) const_cast<Transform*>(this)->updateVectors();
        return m_forward;
    }

    DirectX::XMFLOAT3 Transform::getRight() const {
        if (m_dirty) const_cast<Transform*>(this)->updateVectors();
        return m_right;
    }

    DirectX::XMFLOAT3 Transform::getUp() const {
        if (m_dirty) const_cast<Transform*>(this)->updateVectors();
        return m_up;
    }

    void Transform::updateVectors() {
        float pitch = m_rotation.x;
        float yaw = m_rotation.y;
        float roll = m_rotation.z;

        m_forward.x = sin(yaw) * cos(pitch);
        m_forward.y = sin(pitch);
        m_forward.z = cos(yaw) * cos(pitch);

        DirectX::XMVECTOR fwdVec = DirectX::XMLoadFloat3(&m_forward);
        fwdVec = DirectX::XMVector3Normalize(fwdVec);
        DirectX::XMStoreFloat3(&m_forward, fwdVec);

        m_right.x = cos(yaw);
        m_right.y = 0;
        m_right.z = -sin(yaw);

        DirectX::XMVECTOR rightVec = DirectX::XMLoadFloat3(&m_right);
        DirectX::XMVECTOR upVec = DirectX::XMVector3Cross(fwdVec, rightVec);
        DirectX::XMStoreFloat3(&m_up, upVec);
        
        m_dirty = false;
    }

    DirectX::XMMATRIX Transform::getWorldMatrix() const {
        DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
        DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

        return scale * rotation * translation;
    }

    DirectX::XMFLOAT4X4 Transform::getWorldMatrixFloat() const {
        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, getWorldMatrix());
        return result;
    }

    void Transform::reset() {
        m_position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        m_rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        m_dirty = true;
        updateVectors();
    }

}