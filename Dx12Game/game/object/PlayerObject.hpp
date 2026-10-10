#pragma once

#ifndef DX12ENGINE_PLAYEROBJECT_H
#define DX12ENGINE_PLAYEROBJECT_H

#include <memory>
#include <string>

#include "core/camera/Camera.hpp"
#include "core/engine/Engine.hpp"
#include "core/scene/SceneObject.hpp"

namespace DX12 {
    class Engine;
}

namespace Dx12Game {
    class PlayerObject : public DX12::SceneObject {
    public:
        explicit PlayerObject(DX12::Engine& engine) : m_engine(engine) {}

        void update(float deltaTime) override;
        void onAdd() override;

        void setTag(const std::string& tag) { m_tag = tag; }
        const std::string& getTag() const { return m_tag; }

    protected:
        std::string m_tag = "PlayerTag";

    private:
        DX12::Engine& m_engine;

        float speed = 2.0f;
    };
}

#endif //DX12ENGINE_PLAYEROBJECT_H
