#include "ModelLoader.hpp"

#include <cmath>
#include <fstream>
#include <sstream>

namespace DX12
{
    DirectX::XMFLOAT3 normalize(const DirectX::XMFLOAT3& v) {
        const float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len == 0.0f) return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        return DirectX::XMFLOAT3(v.x / len, v.y / len, v.z / len);
    }

    MeshData ModelLoader::createPyramid()
    {
        MeshData data;
        data.name = "Pyramid";

        data.vertices = {
            { DirectX::XMFLOAT3(0.0f, -0.5f, 0.7f),
              DirectX::XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f),
              DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },

            { DirectX::XMFLOAT3(-0.6f, -0.5f, -0.4f),
              DirectX::XMFLOAT4(0.3f, 1.0f, 0.3f, 1.0f),
              DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },

            { DirectX::XMFLOAT3(0.6f, -0.5f, -0.4f),
              DirectX::XMFLOAT4(0.3f, 0.3f, 1.0f, 1.0f),
              DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) },

            { DirectX::XMFLOAT3(0.0f, 0.7f, 0.0f),
              DirectX::XMFLOAT4(1.0f, 0.8f, 0.2f, 1.0f),
              DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }
        };

        data.indices = {
            0, 2, 1,
            0, 1, 3,
            1, 2, 3,
            2, 0, 3
        };

        return data;
    }

    MeshData ModelLoader::createCube(const float size) {
        MeshData data;
        data.name = "Cube";

        float half = size * 0.5f;

        const std::vector<DirectX::XMFLOAT3> positions = {
            {-half, -half, -half},
            {-half, -half,  half},
            { half, -half,  half},
            { half, -half, -half},
            {-half,  half, -half},
            {-half,  half,  half},
            { half,  half,  half},
            { half,  half, -half}
        };

        const std::vector<DirectX::XMFLOAT4> colors = {
            {1.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f, 1.0f}
        };

        const uint32_t indices[] = {
            1, 2, 6, 1, 6, 5,
            0, 4, 7, 0, 7, 3,
            0, 1, 5, 0, 5, 4,
            3, 7, 6, 3, 6, 2,
            4, 5, 6, 4, 6, 7,
            0, 3, 2, 0, 2, 1
        };

        for (int i = 0; i < 36; i++) {
            uint32_t idx = indices[i];
            int faceIndex = i / 6;

            Vertex vert;
            vert.position = positions[idx];
            vert.color = colors[faceIndex];
            vert.normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

            data.vertices.push_back(vert);
            data.indices.push_back(i);
        }

        calculateNormals(data);
        return data;
    }

    MeshData ModelLoader::createSphere(const float radius, const int segments) {
        MeshData data;
        data.name = "Sphere";

        for (int i = 0; i <= segments; i++) {
            float phi = DirectX::XM_PI * i / segments;
            float y = radius * cos(phi);
            float sinPhi = sin(phi);

            for (int j = 0; j <= segments; j++) {
                float theta = 2.0f * DirectX::XM_PI * j / segments;
                float x = radius * sinPhi * cos(theta);
                float z = radius * sinPhi * sin(theta);

                Vertex vert;
                vert.position = DirectX::XMFLOAT3(x, y, z);

                vert.color = DirectX::XMFLOAT4(
                    (x / radius + 1.0f) * 0.5f,
                    (y / radius + 1.0f) * 0.5f,
                    (z / radius + 1.0f) * 0.5f,
                    1.0f
                );

                vert.normal = DirectX::XMFLOAT3(x, y, z);
                vert.normal = normalize(vert.normal);

                data.vertices.push_back(vert);
            }
        }

        for (int i = 0; i < segments; i++) {
            for (int j = 0; j < segments; j++) {
                int idx = i * (segments + 1) + j;

                data.indices.push_back(idx);
                data.indices.push_back(idx + 1);
                data.indices.push_back(idx + segments + 1);

                data.indices.push_back(idx + 1);
                data.indices.push_back(idx + segments + 2);
                data.indices.push_back(idx + segments + 1);
            }
        }

        return data;
    }

    MeshData ModelLoader::createPlane(const float width, const float depth) {
        MeshData data;
        data.name = "Plane";

        float halfW = width * 0.5f;
        float halfD = depth * 0.5f;

        data.vertices = {
            { DirectX::XMFLOAT3(-halfW, 0.0f, -halfD), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { DirectX::XMFLOAT3( halfW, 0.0f, -halfD), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { DirectX::XMFLOAT3( halfW, 0.0f,  halfD), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-halfW, 0.0f,  halfD), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }
        };

        data.indices = {
            0, 1, 2,
            0, 2, 3
        };

        return data;
    }

    void ModelLoader::calculateNormals(MeshData& meshData)
    {
        for (auto& vert : meshData.vertices)
        {
            vert.normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        }

        for (size_t i = 0; i < meshData.indices.size(); i += 3) {
            Vertex& v1 = meshData.vertices[meshData.indices[i]];
            Vertex& v2 = meshData.vertices[meshData.indices[i + 1]];
            Vertex& v3 = meshData.vertices[meshData.indices[i + 2]];

            const DirectX::XMFLOAT3 normal = calculateFaceNormal(v1, v2, v3);

            v1.normal = normalize(DirectX::XMFLOAT3(
                v1.normal.x + normal.x,
                v1.normal.y + normal.y,
                v1.normal.z + normal.z
            ));

            v2.normal = normalize(DirectX::XMFLOAT3(
                v2.normal.x + normal.x,
                v2.normal.y + normal.y,
                v2.normal.z + normal.z
            ));

            v3.normal = normalize(DirectX::XMFLOAT3(
                v3.normal.x + normal.x,
                v3.normal.y + normal.y,
                v3.normal.z + normal.z
            ));
        }

        for (auto& vert : meshData.vertices) {
            vert.normal = normalize(vert.normal);
        }
    }

    DirectX::XMFLOAT3 ModelLoader::calculateFaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
        DirectX::XMFLOAT3 u, w;

        u.x = v2.position.x - v1.position.x;
        u.y = v2.position.y - v1.position.y;
        u.z = v2.position.z - v1.position.z;

        w.x = v3.position.x - v1.position.x;
        w.y = v3.position.y - v1.position.y;
        w.z = v3.position.z - v1.position.z;

        DirectX::XMFLOAT3 normal;
        normal.x = u.y * w.z - u.z * w.y;
        normal.y = u.z * w.x - u.x * w.z;
        normal.z = u.x * w.y - u.y * w.x;

        return normalize(normal);
    }

    MeshData ModelLoader::loadFromOBJ(const std::string& filename) {
        MeshData data;
        // TODO: OBJ uploading
        return data;
    }
}