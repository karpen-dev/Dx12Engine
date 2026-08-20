#include "Scene.hpp"
#include "core/ConsoleLogger.hpp"

namespace DX12 {
    bool Scene::removeObject(const std::string& name) {
        auto it = m_objectMap.find(name);
        if (it == m_objectMap.end()) {
            return false;
        }

        it->second->onRemove();

        for (auto objIt = m_objects.begin(); objIt != m_objects.end(); ++objIt) {
            if (objIt->get() == it->second) {
                m_objects.erase(objIt);
                break;
            }
        }

        m_objectMap.erase(it);
        return true;
    }

    void Scene::clear() {
        for (auto& obj : m_objects) {
            obj->onRemove();
        }
        m_objects.clear();
        m_objectMap.clear();
        ConsoleLogger::info("[SCENE] Cleared all objects");
    }

    bool Scene::loadTestScene(GraphicsDevice& device) {
        auto* pyramid = addMeshRenderer("Pyramid");
        if (!pyramid || !pyramid->createMesh(device, MeshType::Pyramid)) {
            ConsoleLogger::error("Failed to create Pyramid");
            return false;
        }

        pyramid->getTransform().setPosition(0.0f, 0.0f, 0.0f);
        pyramid->getTransform().setRotation(0.0f, 0.0f, 0.0f);
        ConsoleLogger::success("Added Pyramid at (0, 0, 0)");

        ConsoleLogger::success("Test scene loaded successfully! Total objects: " +
                              std::to_string(getObjectCount()));
        return true;
    }

    void Scene::update(float deltaTime) {
        for (auto& obj : m_objects) {
            if (obj->isActive()) {
                obj->update(deltaTime);
            }
        }
    }

    void Scene::render(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) {
        for (auto& obj : m_objects) {
            if (obj->isActive()) {
                obj->render(device, viewProjection);
                obj->draw(device, viewProjection);
            }
        }
    }
}