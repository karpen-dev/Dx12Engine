#include "Camera.hpp"

namespace DX12 {

    Camera::Camera() : m_rotationAngle(0.0f), m_aspectRatio(1.0f) {
        DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
        DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
    }

    void Camera::update(const float deltaTime) {
        m_rotationAngle += deltaTime * 1.0f;

        DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 1.0f, -3.0f, 0.0f);
        DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixLookAtLH(eye, at, up));

        DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(45.0f), m_aspectRatio, 0.1f, 100.0f));
    }

    DirectX::XMFLOAT4X4 Camera::getViewProjectionMatrix() const {
        DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
        DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, at, up);

        DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XMConvertToRadians(45.0f), m_aspectRatio, 0.1f, 100.0f);

        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(m_rotationAngle);

        DirectX::XMMATRIX mvp = rotation * view * proj;

        mvp = DirectX::XMMatrixTranspose(mvp);

        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, mvp);

        return result;
    }

    DirectX::XMFLOAT4X4 Camera::getModelViewMatrix() const {
        DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
        DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, at, up);

        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(m_rotationAngle);

        DirectX::XMMATRIX modelView = rotation * view;

        modelView = DirectX::XMMatrixTranspose(modelView);

        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, modelView);

        return result;
    }

    void Camera::setAspectRatio(float aspect) {
        m_aspectRatio = aspect;
    }

}