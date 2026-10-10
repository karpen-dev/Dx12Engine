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
        auto& t = getTransform();

        auto p = t.getPosition();
        if (window.isKeyDown('W')) p.z += speed * deltaTime;
        if (window.isKeyDown('S')) p.z -= speed * deltaTime;
        if (window.isKeyDown('A')) p.x -= speed * deltaTime;
        if (window.isKeyDown('D')) p.x += speed * deltaTime;
        t.setPosition(p.x, p.y, p.z);

        auto camera = m_engine.getCamera();
        camera.setPosition(p.x, p.y + 5.0f, p.z - 10.0f);
        camera.setTarget(p.x, p.y, p.z);

        ConsoleLogger::info("updated");
    }

}
