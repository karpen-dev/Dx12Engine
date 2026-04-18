#include "./Preloader.hpp"

#include <comdef.h>
#include <iostream>
#include <string>
#include <Windows.h>

#include "../ConsoleLogger.hpp"
#include "./core/engine/Engine.hpp"
#include "core/systemInfoLogger/SystemInfoLogger.h"

namespace DX12
{
    void Preloader::checkDebugLayer()
    {
        ConsoleLogger::info("=== Debug Configuration ===");

    #ifdef _DEBUG
        ConsoleLogger::success("  Debug build: YES");

        ComPtr<ID3D12Debug> debugController;
        const auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

        if (SUCCEEDED(hr)) {
            ConsoleLogger::success("  D3D12 Debug Layer: Available");
        } else {
            ConsoleLogger::warning("  D3D12 Debug Layer: Not available (install Graphics Tools from Windows Features)");
            ConsoleLogger::info("    To enable: Settings -> Apps -> Optional Features -> Add Feature -> Graphics Tools");
        }
    #else
        ConsoleLogger::info("  Release build: YES");
        ConsoleLogger::info("  D3D12 Debug Layer: Disabled (use Debug build for validation)");
    #endif

        std::cout << std::endl;
    }

    bool Preloader::testDirectX12Creation()
    {
        ConsoleLogger::info("=== DirectX 12 Device Test ===");

        ComPtr<ID3D12Device> testDevice;

        ComPtr<IDXGIFactory4> factory;
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        if (FAILED(hr)) {
            ConsoleLogger::error("Failed to create DXGI factory");
            return false;
        }

        ComPtr<IDXGIAdapter1> adapter;
        hr = factory->EnumAdapters1(0, &adapter);
        if (FAILED(hr)) {
            ConsoleLogger::error("Failed to enumerate adapters");
            return false;
        }

        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            ConsoleLogger::warning("First adapter is software adapter, trying to find hardware adapter...");

            for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
                adapter->GetDesc1(&desc);
                if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
                    break;
                }
                adapter.Reset();
            }
        }

        if (!adapter) {
            ConsoleLogger::error("No hardware adapter found");
            return false;
        }

        auto sysInfo = SystemInfo();
        sysInfo.init(desc.VendorId);
        sysInfo.printSystemInfo();
        sysInfo.printGraphicsAdapters();

        hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&testDevice));

        if (SUCCEEDED(hr)) {
            ConsoleLogger::success("DirectX 12 device created successfully");

            D3D12_FEATURE_DATA_D3D12_OPTIONS features = {};
            hr = testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &features, sizeof(features));

            if (SUCCEEDED(hr)) {
                std::cout << "\n  Device Capabilities:" << std::endl;
                std::cout << "    Resource Binding Tier: " << static_cast<int>(features.ResourceBindingTier) << std::endl;
                std::cout << "    Tiled Resources Tier:  " << static_cast<int>(features.TiledResourcesTier) << std::endl;

                if (features.DoublePrecisionFloatShaderOps) {
                    ConsoleLogger::debug("    Double precision support: YES");
                }

                if (features.OutputMergerLogicOp) {
                    ConsoleLogger::debug("    Logic operations support: YES");
                }
            }

            D3D_FEATURE_LEVEL levels[] = {
                D3D_FEATURE_LEVEL_12_1,
                D3D_FEATURE_LEVEL_12_0,
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0
            };

            D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels = {};
            featureLevels.NumFeatureLevels = _countof(levels);
            featureLevels.pFeatureLevelsRequested = levels;

            hr = testDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));

            if (SUCCEEDED(hr)) {
                std::string maxLevel;
                switch (featureLevels.MaxSupportedFeatureLevel) {
                    case D3D_FEATURE_LEVEL_12_1: maxLevel = "12.1"; break;
                    case D3D_FEATURE_LEVEL_12_0: maxLevel = "12.0"; break;
                    case D3D_FEATURE_LEVEL_11_1: maxLevel = "11.1"; break;
                    case D3D_FEATURE_LEVEL_11_0: maxLevel = "11.0"; break;
                    default: maxLevel = "Unknown";
                }
                std::cout << "    Max Feature Level:   " << maxLevel << std::endl;
            }

            std::cout << std::endl;
            return true;
        } else {
            _com_error err(hr);
            ConsoleLogger::error("Failed to create DirectX 12 device");
            ConsoleLogger::error(std::string("  Error: ") + err.ErrorMessage());
            return false;
        }
    }

    void Preloader::setupConsole()
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        DWORD consoleMode;
        GetConsoleMode(hConsole, &consoleMode);
        SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        SetConsoleTitleW(L"DirectX 12 Engine - Debug Console");

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);

        COORD newSize;
        newSize.X = csbi.dwMaximumWindowSize.X;
        newSize.Y = 9999;
        SetConsoleScreenBufferSize(hConsole, newSize);
    }

}

