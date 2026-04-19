#include "Camera.hpp"

namespace DX12 {

    Camera::Camera() : m_rotationAngle(0.0f), m_aspectRatio(1.0f) {
        DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
        DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
    }

    void Camera::update(const float deltaTime) {
        m_rotationAngle += deltaTime * 1.0f;

        DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
        DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixLookAtLH(eye, at, up));

        DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(60.0f), m_aspectRatio, 0.1f, 100.0f));
    }

    DirectX::XMFLOAT4X4 Camera::getMVPForObject(const DirectX::XMFLOAT4X4& worldMatrix) const {
        DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&worldMatrix);
        DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_viewMatrix);
        DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&m_projectionMatrix);

        DirectX::XMMATRIX mvp = world * view * proj;

        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, mvp);
        return result;
    }

    DirectX::XMFLOAT4X4 Camera::getViewProjectionMatrix() const {
        DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_viewMatrix);
        DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&m_projectionMatrix);

        DirectX::XMMATRIX viewProj = view * proj;

        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, viewProj);
        return result;
    }

    DirectX::XMFLOAT4X4 Camera::getModelViewMatrix() const {
        DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_viewMatrix);
        DirectX::XMMATRIX modelView = view;

        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, modelView);
        return result;
    }

    void Camera::setAspectRatio(float aspect) {
        m_aspectRatio = aspect;
    }

    const DirectX::XMFLOAT4X4& Camera::getViewMatrix() const { return m_viewMatrix; }
    const DirectX::XMFLOAT4X4& Camera::getProjectionMatrix() const { return m_projectionMatrix; }
}