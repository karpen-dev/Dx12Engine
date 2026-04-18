#include "GraphicsDevice.hpp"

#include "../d3dx12.h"

#include <cassert>
#include <dxgi1_6.h>

#pragma comment(lib, "dxguid.lib")

namespace DX12
{
    GraphicsDevice::GraphicsDevice()
        : m_rtvDescriptorSize(0)
        , m_dsvDescriptorSize(0)
        , m_frameIndex(0)
        , m_currentFenceValue(0)
        , m_fenceEvent(nullptr)
        , m_isCommandListClosed(true)
        , m_isRecording(false)
        , m_window(nullptr) {
    }

    GraphicsDevice::~GraphicsDevice() {
        shutdown();
    }

    bool GraphicsDevice::initialize(Window& window) {
        m_window = &window;

        if (!createDevice()) {
            OutputDebugStringA("Failed to create device\n");
            return false;
        }

        if (!createCommandObjects()) {
            OutputDebugStringA("Failed to create command objects\n");
            return false;
        }

        if (!createSwapChain(window.getHandle())) {
            OutputDebugStringA("Failed to create swap chain\n");
            return false;
        }

        if (!createRTVDescriptors()) {
            OutputDebugStringA("Failed to create RTV descriptors\n");
            return false;
        }

        if (!createDepthStencil()) {
            OutputDebugStringA("Failed to create depth stencil\n");
            return false;
        }

        if (!createFenceAndEvent()) {
            OutputDebugStringA("Failed to create fence and event\n");
            return false;
        }

        return true;
    }

    void GraphicsDevice::shutdown() {
        waitForGPU();

        if (m_fenceEvent) {
            CloseHandle(m_fenceEvent);
            m_fenceEvent = nullptr;
        }

        m_renderTargets[0].Reset();
        m_renderTargets[1].Reset();
        m_depthStencil.Reset();
        m_rtvHeap.Reset();
        m_dsvHeap.Reset();
        m_swapChain.Reset();
        m_commandList.Reset();
        m_commandAllocator.Reset();
        m_commandQueue.Reset();
        m_fence.Reset();
        m_device.Reset();
    }

    bool GraphicsDevice::createDevice() {
        UINT dxgiFactoryFlags = 0;

#ifdef _DEBUG
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif

        ComPtr<IDXGIFactory4> factory;
        HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
        if (FAILED(hr)) return false;

        ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0,
                                            IID_PPV_ARGS(&m_device)))) {
                break;
                                            }
        }

        if (!m_device) {
            ComPtr<IDXGIAdapter> warpAdapter;
            factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
            hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0,
                                   IID_PPV_ARGS(&m_device));
            if (FAILED(hr)) return false;
        }

        return true;
    }

    bool GraphicsDevice::createCommandObjects() {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        HRESULT hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
        if (FAILED(hr)) return false;

        hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              IID_PPV_ARGS(&m_commandAllocator));
        if (FAILED(hr)) return false;

        hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                         m_commandAllocator.Get(), nullptr,
                                         IID_PPV_ARGS(&m_commandList));
        if (FAILED(hr)) return false;

        hr = m_commandList->Close();
        if (FAILED(hr)) return false;
        m_isCommandListClosed = true;

        return true;
    }

    bool GraphicsDevice::createSwapChain(HWND hwnd) {
        ComPtr<IDXGIFactory4> factory;
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        if (FAILED(hr)) {
            ComPtr<IDXGIDevice> dxgiDevice;
            hr = m_device.As(&dxgiDevice);
            if (SUCCEEDED(hr)) {
                ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr)) {
                    hr = adapter->GetParent(IID_PPV_ARGS(&factory));
                }
            }
        }
        if (FAILED(hr)) return false;

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Width = m_window->getWidth();
        swapChainDesc.Height = m_window->getHeight();
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain1;
        hr = factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        );

        if (FAILED(hr)) return false;

        hr = swapChain1.As(&m_swapChain);
        if (FAILED(hr)) return false;

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

        return true;
    }

    bool GraphicsDevice::createRTVDescriptors() {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        HRESULT hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
        if (FAILED(hr)) return false;

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

        for (UINT i = 0; i < 2; ++i) {
            hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            if (FAILED(hr)) return false;

            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.ptr += m_rtvDescriptorSize;
        }

        return true;
    }

    bool GraphicsDevice::createDepthStencil() {
        D3D12_RESOURCE_DESC depthStencilDesc = {};
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = m_window->getWidth();
        depthStencilDesc.Height = m_window->getHeight();
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

        HRESULT hr = m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_depthStencil)
        );

        if (FAILED(hr)) return false;

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));
        if (FAILED(hr)) return false;

        m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        m_device->CreateDepthStencilView(m_depthStencil.Get(), nullptr, dsvHandle);

        return true;
    }

    bool GraphicsDevice::createFenceAndEvent() {
        HRESULT hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        if (FAILED(hr)) return false;

        m_currentFenceValue = 0;

        m_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        if (!m_fenceEvent) return false;

        return true;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDevice::getCurrentRTV() const {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += m_frameIndex * m_rtvDescriptorSize;
        return rtvHandle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDevice::getDSV() const {
        return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    }

    void GraphicsDevice::beginFrame() {
        HRESULT hr = m_commandAllocator->Reset();
        assert(SUCCEEDED(hr));

        hr = m_commandList->Reset(m_commandAllocator.Get(), nullptr);
        assert(SUCCEEDED(hr));

        m_isRecording = true;
        m_isCommandListClosed = false;

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        m_commandList->ResourceBarrier(1, &barrier);

        CD3DX12_VIEWPORT viewport(0.0f, 0.0f,
                                  static_cast<float>(m_window->getWidth()),
                                  static_cast<float>(m_window->getHeight()));
        CD3DX12_RECT scissorRect(0, 0, m_window->getWidth(), m_window->getHeight());

        m_commandList->RSSetViewports(1, &viewport);
        m_commandList->RSSetScissorRects(1, &scissorRect);

        const float clearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
        m_commandList->ClearRenderTargetView(getCurrentRTV(), clearColor, 0, nullptr);
        m_commandList->ClearDepthStencilView(getDSV(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = getCurrentRTV();
        const D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = getDSV();

        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    }

    void GraphicsDevice::endFrame() {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        );

        m_commandList->ResourceBarrier(1, &barrier);

        HRESULT hr = m_commandList->Close();
        assert(SUCCEEDED(hr));

        m_isRecording = false;
        m_isCommandListClosed = true;
    }

    void GraphicsDevice::present(bool vsync) {
        ID3D12CommandList* commandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        HRESULT hr = m_swapChain->Present(vsync ? 1 : 0, 0);
        assert(SUCCEEDED(hr));

        waitForGPU();

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    void GraphicsDevice::waitForGPU() {
        m_currentFenceValue++;
        HRESULT hr = m_commandQueue->Signal(m_fence.Get(), m_currentFenceValue);
        assert(SUCCEEDED(hr));

        if (m_fence->GetCompletedValue() < m_currentFenceValue) {
            hr = m_fence->SetEventOnCompletion(m_currentFenceValue, m_fenceEvent);
            assert(SUCCEEDED(hr));
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    void GraphicsDevice::flushCommandQueue() {
        waitForGPU();

        HRESULT hr = m_commandAllocator->Reset();
        assert(SUCCEEDED(hr));

        hr = m_commandList->Reset(m_commandAllocator.Get(), nullptr);
        assert(SUCCEEDED(hr));

        hr = m_commandList->Close();
        assert(SUCCEEDED(hr));

        m_isRecording = false;
        m_isCommandListClosed = true;
    }

    ComPtr<ID3D12Resource> GraphicsDevice::createUploadBuffer(UINT64 byteSize) const
    {
        ComPtr<ID3D12Resource> uploadBuffer;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

        HRESULT hr = m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadBuffer)
        );

        return (SUCCEEDED(hr)) ? uploadBuffer : nullptr;
    }

    ComPtr<ID3D12Resource> GraphicsDevice::createDefaultBuffer(UINT64 byteSize) {
        ComPtr<ID3D12Resource> defaultBuffer;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

        HRESULT hr = m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&defaultBuffer)
        );

        return (SUCCEEDED(hr)) ? defaultBuffer : nullptr;
    }

    ComPtr<ID3D12Resource> GraphicsDevice::createDefaultBuffer(const void* initData, UINT64 byteSize,
                                                               ComPtr<ID3D12Resource>& uploadBuffer) {
        auto defaultBuffer = createDefaultBuffer(byteSize);
        if (!defaultBuffer) return nullptr;

        uploadBuffer = createUploadBuffer(byteSize);
        if (!uploadBuffer) return nullptr;

        void* mappedData;
        CD3DX12_RANGE readRange(0, 0);
        HRESULT hr = uploadBuffer->Map(0, &readRange, &mappedData);
        if (FAILED(hr)) return nullptr;

        memcpy(mappedData, initData, byteSize);
        uploadBuffer->Unmap(0, nullptr);

        copyBuffer(defaultBuffer, uploadBuffer, byteSize);

        return defaultBuffer;
    }

    void GraphicsDevice::transitionResource(ComPtr<ID3D12Resource> resource,
                                           D3D12_RESOURCE_STATES before,
                                           D3D12_RESOURCE_STATES after)
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(), before, after);

        m_commandList->ResourceBarrier(1, &barrier);
    }

    void GraphicsDevice::copyBuffer(ComPtr<ID3D12Resource> dest,
                                   ComPtr<ID3D12Resource> src,
                                   UINT64 byteSize) const
    {
        m_commandList->CopyBufferRegion(dest.Get(), 0, src.Get(), 0, byteSize);
    }
}