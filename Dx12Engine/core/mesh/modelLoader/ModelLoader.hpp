#pragma once

#include "../../Types.hpp"
#include <vector>
#include <string>

namespace DX12
{
    struct MeshData
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::string name;

        void clear()
        {
            vertices.clear();
            indices.clear();
            name.clear();
        }

        bool isEmpty() const
        {
            return vertices.empty() || indices.empty();
        }
    };

    class ModelLoader
    {
    public:
        ModelLoader() = default;
        ~ModelLoader() = default;

        static MeshData createPyramid();
        static MeshData createCube(float size = 1.0f);
        static MeshData createSphere(float radius, int segments);
        static MeshData createPlane(float width, float depth);

        static MeshData loadFromOBJ(const std::string& filename);

    private:
        static void calculateNormals(MeshData& meshData);
        static DirectX::XMFLOAT3 calculateFaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3);
    };
}