#pragma once

#include "../Types.hpp"
#include "../graphicsDevice/GraphicsDevice.hpp"
#include "../shader/Shader.hpp"

namespace DX12 {

    class Pipeline {
    public:
        Pipeline();
        ~Pipeline();

        bool initialize(GraphicsDevice& device, Shader& shader);
        void apply(const GraphicsDevice& device) const;

        ComPtr<ID3D12RootSignature> getRootSignature() const { return m_rootSignature; }
        ComPtr<ID3D12PipelineState> getPipelineState() const { return m_pipelineState; }

    private:
        bool createRootSignature(GraphicsDevice& device);
        bool createPipelineState(GraphicsDevice& device, Shader& shader);

        ComPtr<ID3D12RootSignature> m_rootSignature;
        ComPtr<ID3D12PipelineState> m_pipelineState;
    };

}