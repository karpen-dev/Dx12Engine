#pragma once

#include "../Types.hpp"
#include "../window/Window.hpp"

#include <dxgi1_6.h>

namespace DX12 {
    class Window;

class GraphicsDevice {
    public:
        GraphicsDevice();
        ~GraphicsDevice();

        bool initialize(Window& window);
        void shutdown();

        ComPtr<ID3D12Device> getDevice() const { return m_device; }
        ComPtr<ID3D12CommandQueue> getCommandQueue() const { return m_commandQueue; }
        ComPtr<ID3D12CommandAllocator> getCommandAllocator() const { return m_commandAllocator; }
        ComPtr<ID3D12GraphicsCommandList> getCommandList() const { return m_commandList; }
        ComPtr<IDXGISwapChain3> getSwapChain() const { return m_swapChain; }
        ComPtr<ID3D12DescriptorHeap> getRTVHeap() const { return m_rtvHeap; }
        ComPtr<ID3D12DescriptorHeap> getDSVHeap() const { return m_dsvHeap; }

        UINT getCurrentBackBufferIndex() const { return m_frameIndex; }
        UINT getRTVDescriptorSize() const { return m_rtvDescriptorSize; }
        D3D12_CPU_DESCRIPTOR_HANDLE getCurrentRTV() const;
        D3D12_CPU_DESCRIPTOR_HANDLE getDSV() const;

        void beginFrame();
        void endFrame();
        void present(bool vsync = true);
        void waitForGPU();
        void flushCommandQueue();

        ComPtr<ID3D12Resource> createDefaultBuffer(const void* initData, UINT64 byteSize,
                                                    ComPtr<ID3D12Resource>& uploadBuffer);
        ComPtr<ID3D12Resource> createUploadBuffer(UINT64 byteSize) const;
        ComPtr<ID3D12Resource> createDefaultBuffer(UINT64 byteSize);

        void transitionResource(ComPtr<ID3D12Resource> resource,
                               D3D12_RESOURCE_STATES before,
                               D3D12_RESOURCE_STATES after);

        void copyBuffer(ComPtr<ID3D12Resource> dest, ComPtr<ID3D12Resource> src, UINT64 byteSize) const;

    private:
        bool createDevice();
        bool createCommandObjects();
        bool createSwapChain(HWND hwnd);
        bool createRTVDescriptors();
        bool createDepthStencil();
        bool createFenceAndEvent();

        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        ComPtr<ID3D12Resource> m_renderTargets[2];
        ComPtr<ID3D12Resource> m_depthStencil;

        UINT m_rtvDescriptorSize;
        UINT m_dsvDescriptorSize;

        int m_frameIndex;
        UINT64 m_currentFenceValue;
        HANDLE m_fenceEvent;
        ComPtr<ID3D12Fence> m_fence;

        bool m_isCommandListClosed;
        bool m_isRecording;

        Window* m_window;
    };
}