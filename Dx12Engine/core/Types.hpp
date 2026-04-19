#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include <string>

using Microsoft::WRL::ComPtr;

namespace DX12 {
	using uint32 = unsigned int;
	using float32 = float;

	enum class MeshType
	{
		Pyramid,
		Cube,
		Sphere,
		Plane
	};

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 normal;
	};

	struct TransformCB {
		DirectX::XMFLOAT4X4 modelViewProjection;
		DirectX::XMFLOAT4X4 modelView;
		DirectX::XMFLOAT3 lightDirection;
		float time;
		float padding[4];
	};

	struct ShaderBlob
	{
		void* data;
		size_t size;
		std::string error;
		bool isValid() const {
			return data != nullptr && error.empty();
		}
	};
}