#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "./meshRenderer/MeshRenderer.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"

namespace DX12 {

    class Scene {
    public:
        Scene();
        ~Scene();

        MeshRenderer* addObject(const std::string& name);
        MeshRenderer* getObject(const std::string& name);

        bool removeObject(const std::string& name);
        void clear();

        bool loadTestScene(const GraphicsDevice& device);
        bool loadFromFile(const GraphicsDevice& device, const std::string& filename);

        void render(const GraphicsDevice& device, const DirectX::XMMATRIX& viewProjection);

        size_t getObjectCount() const { return m_objects.size(); }
        const std::vector<std::unique_ptr<MeshRenderer>>& getAllObjects() const { return m_objects; }
        
    private:
        std::vector<std::unique_ptr<MeshRenderer>> m_objects;
        std::unordered_map<std::string, MeshRenderer*> m_objectMap;
    };

}