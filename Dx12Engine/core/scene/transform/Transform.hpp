#pragma once

#include <DirectXMath.h>

namespace DX12
{
    class Transform
    {
    public:
        Transform();

        void setPosition(float x, float y, float z);
        void setRotation(float pith, float yaw, float roll);
        void setScale(float x, float y, float z);

        void translate(float x, float y, float z);
        void rotate(float pith, float yaw, float roll);

        const DirectX::XMFLOAT3& getPosition() const { return m_position; }
        const DirectX::XMFLOAT3& getRotation() const { return m_rotation; }
        const DirectX::XMFLOAT3& getScale() const { return m_scale; }

        DirectX::XMFLOAT4X4 getMatrix() const;

    private:
        DirectX::XMFLOAT3 m_position;
        DirectX::XMFLOAT3 m_rotation;
        DirectX::XMFLOAT3 m_scale;

        mutable bool m_matrixDirty;
        mutable DirectX::XMFLOAT4X4 m_cachedMatrix;
    };
}