#include "PlayerObject.hpp"

namespace Dx12Game {

    void PlayerObject::onAdd() {
        auto camera = std::make_unique<DX12::Camera>();
        const auto width  = static_cast<float>(m_engine.getWindow().getWidth());
        const auto height = static_cast<float>(m_engine.getWindow().getHeight());
        camera->setAspectRatio(width / height);

        m_engine.setCamera(std::move(camera));
    }

    void PlayerObject::update(float deltaTime) {
        auto& window = m_engine.getWindow();
        auto& camera = m_engine.getCamera();
        const auto& mouse = window.getMouse();

        if (mouse.captured) {
            const float sens = 0.15f;
            camera.addYaw( mouse.dx * sens);
            camera.addPitch(-mouse.dy * sens);
        }

        const float yawRad   = DirectX::XMConvertToRadians(camera.getYaw());
        const float pitchRad = DirectX::XMConvertToRadians(camera.getPitch());
        const float cy = cosf(yawRad),   sy = sinf(yawRad);
        const float cp = cosf(pitchRad), sp = sinf(pitchRad);

        DirectX::XMVECTOR lookDir = DirectX::XMVectorSet(
            -sy * cp,  sp,  -cy * cp,  0.0f);

        DirectX::XMVECTOR forward = DirectX::XMVectorSet(-sy, 0.0f, -cy, 0.0f);
        DirectX::XMVECTOR right   = DirectX::XMVectorSet( cy, 0.0f, -sy, 0.0f);

        auto& t = getTransform();
        auto  p = t.getPosition();
        const float s = 2.0f * deltaTime;

        DirectX::XMVECTOR moveDir = DirectX::XMVectorZero();
        if (window.isKeyDown('W')) moveDir = DirectX::XMVectorAdd(moveDir, forward);
        if (window.isKeyDown('S')) moveDir = DirectX::XMVectorSubtract(moveDir, forward);
        if (window.isKeyDown('D')) moveDir = DirectX::XMVectorAdd(moveDir, right);
        if (window.isKeyDown('A')) moveDir = DirectX::XMVectorSubtract(moveDir, right);

        if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(moveDir)) > 1e-6f) {
            moveDir = DirectX::XMVector3Normalize(moveDir);
            DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&p);
            currentPos = DirectX::XMVectorMultiplyAdd(
                moveDir, DirectX::XMVectorReplicate(s), currentPos);
            DirectX::XMStoreFloat3(&p, currentPos);
            t.setPosition(p.x, p.y, p.z);
        }

        t.setRotation(0.0f, -camera.getYaw(), 0.0f);

        constexpr float eyeHeight = 1.7f;
        DirectX::XMFLOAT3 eyePos = { p.x, p.y + eyeHeight, p.z };

        DirectX::XMVECTOR eyeV = DirectX::XMLoadFloat3(&eyePos);
        DirectX::XMVECTOR targetV = DirectX::XMVectorAdd(eyeV, lookDir);
        DirectX::XMFLOAT3 target;
        DirectX::XMStoreFloat3(&target, targetV);

        camera.setPosition(eyePos);
        camera.setTarget(target);
    }

}