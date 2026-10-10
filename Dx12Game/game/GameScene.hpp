#pragma once

#ifndef DX12ENGINE_BASESCENE_H
#define DX12ENGINE_BASESCENE_H

#include <core/scene/Scene.hpp>
#include <core/ConsoleLogger.hpp>

#include "object/PlayerObject.hpp"

namespace Dx12Game {
    class GameScene : public DX12::Scene {
    public:
        GameScene(const GameScene&) = delete;
        GameScene& operator=(const GameScene&) = delete;

        GameScene(GameScene&&) = default;
        GameScene& operator=(GameScene&&) = default;

        explicit GameScene(DX12::Engine& engine) : m_engine(engine) {}

        void load() override {
            ConsoleLogger::info("Loading game scene...");

            ConsoleLogger::info("Getting device...");
            auto& device = m_engine.getGraphicsDevice();

            auto* player = addObject<PlayerObject>("Player", m_engine);
            auto* playerMesh = addMeshRenderer("PlayerVisual");
            if (playerMesh && playerMesh->createMesh(device, DX12::MeshType::Cube)) {
                playerMesh->getTransform().setParent(&player->getTransform());
                playerMesh->getTransform().setPosition(0.0f, 0.0f, 0.0f);
            }

            if (!player) {
                ConsoleLogger::error("Failed to create PlayerObject");
                return;
            }

            auto* pyramid = addMeshRenderer("Pyramid");
            if (!pyramid || !pyramid->createMesh(device, DX12::MeshType::Pyramid)) {
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

            if (auto* pyramid = getObject("Pyramid")) {
                auto& t = pyramid->getTransform();
                float angle = t.getRotation().y + deltaTime * 1.0f;
                t.setRotation(0.0f, angle, 0.0f);
            }
        }

    private:
        DX12::Engine& m_engine;
    };
}

#endif //DX12ENGINE_BASESCENE_H