#pragma once

#ifndef DX12ENGINE_GAMEOBJECT_H
#define DX12ENGINE_GAMEOBJECT_H

#include <core/scene/SceneObject.hpp>

namespace Dx12Game {
    class GameObject : DX12::SceneObject {
    public:
        virtual ~GameObject();

        virtual void onCollision(SceneObject* other) {}
        virtual void onTrigger(SceneObject* other) {}

        void setTag(const std::string& tag) { m_tag = tag; }
        const std::string& getTag() const { return m_tag; }

    protected:
        std::string m_tag = "Default";
    };
}


#endif //DX12ENGINE_GAMEOBJECT_H
