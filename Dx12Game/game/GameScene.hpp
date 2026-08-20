#pragma once

#ifndef DX12ENGINE_BASESCENE_H
#define DX12ENGINE_BASESCENE_H

#include <core/scene/Scene.hpp>
#include <core/ConsoleLogger.hpp>

#include "gameObject/GameObject.hpp"

namespace Dx12Game {
    class GameScene : public DX12::Scene {
    public:
        GameScene(const GameScene&) = delete;
        GameScene& operator=(const GameScene&) = delete;

        GameScene(GameScene&&) = default;
        GameScene& operator=(GameScene&&) = default;

        explicit GameScene(DX12::GraphicsDevice& device) : m_device(device) {}

        void load() override {
            ConsoleLogger::info("Loading game scene...");

            auto* pyramid = addMeshRenderer("Pyramid");
            if (!pyramid || !pyramid->createMesh(m_device, DX12::MeshType::Pyramid)) {
                ConsoleLogger::error("Failed to create Pyramid");
                return;
            }

            pyramid->getTransform().setPosition(0.0f, 0.0f, 0.0f);
            pyramid->getTransform().setRotation(0.0f, 0.0f, 0.0f);
            ConsoleLogger::success("Added Pyramid at (0, 0, 0)");

            ConsoleLogger::success("Test scene loaded successfully! Total objects: " +
                                  std::to_string(getObjectCount()));

            ConsoleLogger::info("Game scene loaded with objects");
        }

        void update(float deltaTime) override {
            Scene::update(deltaTime);
        }

    private:
        DX12::GraphicsDevice& m_device;
    };
}

#endif //DX12ENGINE_BASESCENE_H
