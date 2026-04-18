#include "Mesh.hpp"

#include <iostream>
#include <DirectXMath.h>

#include "../d3dx12.h"

namespace DX12 {

    DirectX::XMFLOAT3 normalize(const DirectX::XMFLOAT3& v) {
        const float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len == 0.0f) return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        return DirectX::XMFLOAT3(v.x / len, v.y / len, v.z / len);
    }

    Mesh::Mesh() = default;
    Mesh::~Mesh() = default;

    bool Mesh::createTriangle(const GraphicsDevice& device) {
        std::cout << "[DEBUG] Creating pyramid..." << std::endl;

        Vertex vertices[] = {
            { DirectX::XMFLOAT3(0.0f, -0.5f, 0.7f),
              DirectX::XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f),
              DirectX::XMFLOAT3(0.0f, -0.5f, 0.8f) },

            { DirectX::XMFLOAT3(-0.6f, -0.5f, -0.4f),
              DirectX::XMFLOAT4(0.3f, 1.0f, 0.3f, 1.0f),
              DirectX::XMFLOAT3(-0.7f, -0.5f, 0.2f) },

            { DirectX::XMFLOAT3(0.6f, -0.5f, -0.4f),
              DirectX::XMFLOAT4(0.3f, 0.3f, 1.0f, 1.0f),
              DirectX::XMFLOAT3(0.7f, -0.5f, 0.2f) },

            { DirectX::XMFLOAT3(0.0f, 0.7f, 0.0f),
              DirectX::XMFLOAT4(1.0f, 0.8f, 0.2f, 1.0f),
              DirectX::XMFLOAT3(0.0f, 0.8f, 0.0f) }
        };

        constexpr auto baseNormal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

        vertices[0].normal = baseNormal;
        vertices[1].normal = baseNormal;
        vertices[2].normal = baseNormal;

        vertices[3].normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

        m_vertexCount = 4;

        const uint32 indices[] = {
            0, 2, 1,
            0, 1, 3,
            1, 2, 3,
            2, 0, 3
        };

        m_indexCount = 12;

        for (int i = 0; i < m_vertexCount; i++) {
            std::cout << "  Vertex " << i << ": pos("
                      << vertices[i].position.x << ", "
                      << vertices[i].position.y << ", "
                      << vertices[i].position.z << ") color("
                      << vertices[i].color.x << ", "
                      << vertices[i].color.y << ", "
                      << vertices[i].color.z << ")" << std::endl;
        }

        constexpr UINT vertexBufferSize = sizeof(vertices);

        const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

        HRESULT hr = device.getDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)
        );

        if (FAILED(hr)) {
            std::cerr << "[ERROR] Failed to create vertex buffer" << std::endl;
            return false;
        }

        void* mappedData;
        CD3DX12_RANGE readRange(0, 0);
        hr = m_vertexBuffer->Map(0, &readRange, &mappedData);
        if (FAILED(hr)) {
            std::cerr << "[ERROR] Failed to map vertex buffer" << std::endl;
            return false;
        }

        memcpy(mappedData, vertices, vertexBufferSize);
        m_vertexBuffer->Unmap(0, nullptr);

        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;

        constexpr UINT indexBufferSize = sizeof(indices);
        resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);

        hr = device.getDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_indexBuffer)
        );

        if (FAILED(hr)) {
            std::cerr << "[ERROR] Failed to create index buffer" << std::endl;
            return false;
        }

        hr = m_indexBuffer->Map(0, &readRange, &mappedData);
        if (FAILED(hr)) {
            std::cerr << "[ERROR] Failed to map index buffer" << std::endl;
            return false;
        }

        memcpy(mappedData, indices, indexBufferSize);
        m_indexBuffer->Unmap(0, nullptr);

        m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = indexBufferSize;

        std::cout << "[SUCCESS] Pyramid created. Vertices: " << m_vertexCount
                  << ", Indices: " << m_indexCount << std::endl;

        return true;
    }

    void Mesh::draw(const GraphicsDevice& device) const
    {
        const auto commandList = device.getCommandList();

        commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

        if (m_indexBuffer) {
            commandList->IASetIndexBuffer(&m_indexBufferView);
        }

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if (m_indexCount > 0) {
            commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
        } else {
            commandList->DrawInstanced(m_vertexCount, 1, 0, 0);
        }
    }

}