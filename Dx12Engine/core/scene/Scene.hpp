#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

#include <DirectXMath.h>

#include "SceneObject.hpp"
#include "./meshRenderer/MeshRenderer.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"
#include "core/ConsoleLogger.hpp"
#include "transform/Transform.hpp"

namespace DX12 {

    class Scene {
    public:
        Scene() = default;
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&) = default;
        Scene& operator=(Scene&&) = default;

        template<typename T, typename... Args>
        T* addObject(const std::string& name, Args&&... args) {
            static_assert(std::is_base_of_v<SceneObject, T>, "T must be derived from SceneObject");

            if (m_objectMap.contains(name)) {
                return nullptr;
            }

            auto object = std::make_unique<T>(std::forward<Args>(args)...);
            object->setName(name);

            T* ptr = object.get();

            m_objects.push_back(std::move(object));
            m_objectMap[name] = ptr;

            ptr->onAdd();

            return ptr;
        }

        MeshRenderer* addMeshRenderer(const std::string& name) {
            return addObject<MeshRenderer>(name);
        }

        SceneObject* getObject(const std::string& name) {
            auto it = m_objectMap.find(name);
            return it != m_objectMap.end() ? it->second : nullptr;
        }

        template<typename T>
        T* getObjectAs(const std::string& name) {
            static_assert(std::is_base_of_v<SceneObject, T>, "T must be derived from SceneObject");
            auto obj = getObject(name);
            return dynamic_cast<T*>(obj);
        }

        template<typename T>
        std::vector<T*> getObjectsByType() {
            static_assert(std::is_base_of_v<SceneObject, T>, "T must be derived from SceneObject");
            std::vector<T*> result;
            for (auto& obj : m_objects) {
                if (T* casted = dynamic_cast<T*>(obj.get())) {
                    result.push_back(casted);
                }
            }
            return result;
        }

        bool removeObject(const std::string& name);
        void clear();

        bool loadTestScene(GraphicsDevice& device);

        virtual void load() {}
        virtual void unload() {}
        virtual void update(float deltaTime);
        virtual void render(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection);

        size_t getObjectCount() const { return m_objects.size(); }
        const std::vector<std::unique_ptr<SceneObject>>& getAllObjects() const { return m_objects; }

    protected:
        std::vector<std::unique_ptr<SceneObject>> m_objects;
        std::unordered_map<std::string, SceneObject*> m_objectMap;
    };

}