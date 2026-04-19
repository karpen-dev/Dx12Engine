#include "Scene.hpp"
#include "core/ConsoleLogger.hpp"

#include <iostream>

namespace DX12 {

    Scene::Scene() = default;
    Scene::~Scene() = default;

    MeshRenderer* Scene::addObject(const std::string& name) {
        if (m_objectMap.find(name) != m_objectMap.end()) {
            std::cerr << "[WARNING] Object with name '" << name << "' already exists!" << std::endl;
            return nullptr;
        }

        auto renderer = std::make_unique<MeshRenderer>();
        MeshRenderer* ptr = renderer.get();

        m_objects.push_back(std::move(renderer));
        m_objectMap[name] = ptr;

        std::cout << "[SCENE] Added object: " << name << std::endl;
        return ptr;
    }

    MeshRenderer* Scene::getObject(const std::string& name) {
        auto it = m_objectMap.find(name);
        return it != m_objectMap.end() ? it->second : nullptr;
    }

    bool Scene::removeObject(const std::string& name) {
        auto it = m_objectMap.find(name);
        if (it == m_objectMap.end()) {
            return false;
        }

        for (auto objIt = m_objects.begin(); objIt != m_objects.end(); ++objIt) {
            if (objIt->get() == it->second) {
                m_objects.erase(objIt);
                break;
            }
        }

        m_objectMap.erase(it);
        std::cout << "[SCENE] Removed object: " << name << std::endl;
        return true;
    }

    void Scene::clear() {
        m_objects.clear();
        m_objectMap.clear();
        std::cout << "[SCENE] Cleared all objects" << std::endl;
    }

    bool Scene::loadTestScene(const GraphicsDevice& device) {
        ConsoleLogger::info("Loading test scene...");

        auto* pyramid = addObject("Pyramid");
        if (pyramid && pyramid->createMesh(device, MeshType::Pyramid)) {
            pyramid->getTransform().setPosition(0.0f, 0.0f, 0.0f);
            pyramid->getTransform().setRotation(0.0f, 0.0f, 0.0f);
            ConsoleLogger::success("Added Pyramid at (0, 0, 0)");
        } else {
            ConsoleLogger::error("Failed to create Pyramid");
            return false;
        }

        ConsoleLogger::success("Test scene loaded successfully! Total objects: " +
                              std::to_string(m_objects.size()));
        return true;
    }

    bool Scene::loadFromFile(const GraphicsDevice& device, const std::string& filename) {
        // TODO: Load scene from file

        ConsoleLogger::warning("loadFromFile not implemented yet");
        return false;
    }

    void Scene::render(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection) {
        for (auto& renderer : m_objects) {
            renderer->draw(device, viewProjection);
        }
    }

}
