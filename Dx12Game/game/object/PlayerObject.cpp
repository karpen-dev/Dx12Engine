#include "PlayerObject.hpp"

namespace Dx12Game {
    void PlayerObject::onAdd() {
        auto camera = std::make_unique<DX12::Camera>();

        const auto width = static_cast<float>(m_engine.getWindow().getWidth());
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
        camera.setDistance(camera.clamp(camera.getDistance() - mouse.wheel * 0.5f,
                                      1.0f, 30.0f));

        auto& t = getTransform();
        auto p = t.getPosition();
        const float s = 2.0f * deltaTime;

        float yRad = DirectX::XMConvertToRadians(camera.getYaw());

        DirectX::XMVECTOR forward = DirectX::XMVectorSet(sinf(yRad), 0.0f, -cosf(yRad), 0.0f);
        DirectX::XMVECTOR right   = DirectX::XMVectorSet(-cosf(yRad), 0.0f, sinf(yRad), 0.0f);

        DirectX::XMVECTOR moveDir = DirectX::XMVectorZero();

        if (window.isKeyDown('W')) moveDir = DirectX::XMVectorAdd(moveDir, forward);
        if (window.isKeyDown('S')) moveDir = DirectX::XMVectorSubtract(moveDir, forward);
        if (window.isKeyDown('D')) moveDir = DirectX::XMVectorAdd(moveDir, right);
        if (window.isKeyDown('A')) moveDir = DirectX::XMVectorSubtract(moveDir, right);

        if (DirectX::XMVector3Greater(DirectX::XMVector3LengthSq(moveDir), DirectX::XMVectorZero())) {
            moveDir = DirectX::XMVector3Normalize(moveDir);

            DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&p);
            currentPos = DirectX::XMVectorMultiplyAdd(moveDir, DirectX::XMVectorReplicate(s), currentPos);
            DirectX::XMStoreFloat3(&p, currentPos);
        }

        t.setPosition(p.x, p.y, p.z);
        camera.setTarget(p);
    }

}
