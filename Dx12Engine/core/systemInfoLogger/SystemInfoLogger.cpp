#include "SystemInfoLogger.h"

#include <comdef.h>
#include <dxgi1_4.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>

#include "../ConsoleLogger.hpp"

namespace DX12
{
    SystemInfo::SystemInfo() = default;

    bool SystemInfo::init(uint32_t vendorId)
    {
         // === Version info ===

        std::string version = "Windows ";

        using fnRtlGetVersion = LONG (WINAPI*)(PRTL_OSVERSIONINFOW);
        HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
        if (hNtdll) {
            auto RtlGetVersion = reinterpret_cast<fnRtlGetVersion>(
                GetProcAddress(hNtdll, "RtlGetVersion"));

            if (RtlGetVersion) {
                RTL_OSVERSIONINFOW osvi = {};
                osvi.dwOSVersionInfoSize = sizeof(osvi);
                if (RtlGetVersion(&osvi) == 0) {
                    std::ostringstream oss;
                    oss << osvi.dwMajorVersion << "." << osvi.dwMinorVersion
                        << " (Build " << osvi.dwBuildNumber << ")";
                    version += oss.str();
                }
            }
        }

        m_versionInfo = version;

        // === CPU info

        std::string cpuName = "Unknown";

        int cpuInfo[4] = { 0 };
        __cpuid(cpuInfo, 0x80000000);

        if (static_cast<unsigned int>(cpuInfo[0]) >= 0x80000004) {
            char brand[49] = { 0 };
            __cpuid(cpuInfo, 0x80000002);
            memcpy(brand, cpuInfo, sizeof(cpuInfo));
            __cpuid(cpuInfo, 0x80000003);
            memcpy(brand + 16, cpuInfo, sizeof(cpuInfo));
            __cpuid(cpuInfo, 0x80000004);
            memcpy(brand + 32, cpuInfo, sizeof(cpuInfo));
            cpuName = brand;
        }

        cpuName.erase(0, cpuName.find_first_not_of(" \t"));
        cpuName.erase(cpuName.find_last_not_of(" \t") + 1);

        m_cpuInfo = cpuName;

        // === Total RAM info

        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);
        GlobalMemoryStatusEx(&memoryStatus);

        constexpr size_t GB = 1024ull * 1024 * 1024;
        m_totalRam = memoryStatus.ullTotalPhys / GB;

        // === GPU vendor name ===

        switch (vendorId) {
            case 0x10DE: m_gpuVendorName = "NVIDIA"; break;
            case 0x1002: m_gpuVendorName = "AMD"; break;
            case 0x8086: m_gpuVendorName = "Intel"; break;
            case 0x1414: m_gpuVendorName = "Microsoft"; break;
            default: m_gpuVendorName = "Unknown"; break;
        }

        // === Dx12 adapters info ===

        std::vector<AdapterInfo> adapters;

        ComPtr<IDXGIFactory4> factory;
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        if (FAILED(hr)) {
            ConsoleLogger::error("Failed to create DXGI factory");
            m_DX12Adapters = adapters;
            return false;
        }

        ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            AdapterInfo info;

            std::wstring wname(desc.Description);
            info.name = std::string(wname.begin(), wname.end());
            info.description = info.name;
            info.dedicatedVideoMemory = desc.DedicatedVideoMemory / (1024 * 1024);
            info.dedicatedSystemMemory = desc.DedicatedSystemMemory / (1024 * 1024);
            info.sharedSystemMemory = desc.SharedSystemMemory / (1024 * 1024);
            info.isSoftware = (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;
            info.vendorId = desc.VendorId;
            info.deviceId = desc.DeviceId;

            std::wstring driverVersion;
            ComPtr<IDXGIAdapter3> adapter3;
            if (SUCCEEDED(adapter.As(&adapter3))) {
                LARGE_INTEGER umdVersion;
                if (SUCCEEDED(adapter3->CheckInterfaceSupport(__uuidof(ID3D12Device), &umdVersion))) {
                    std::ostringstream oss;
                    oss << HIWORD(umdVersion.HighPart) << "."
                        << LOWORD(umdVersion.HighPart) << "."
                        << HIWORD(umdVersion.LowPart) << "."
                        << LOWORD(umdVersion.LowPart);
                    info.driverVersion = oss.str();
                }
            }

            if (!info.isSoftware) {
                HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr);
                info.description += (SUCCEEDED(hr)) ? " (D3D12 Compatible)" : " (D3D12 NOT Compatible)";
            } else {
                info.description += " (Software Adapter)";
            }

            adapters.push_back(info);
            adapter.Reset();
        }

        m_DX12Adapters = adapters;

        return true;
    }


    void SystemInfo::printSystemInfo() const
    {
        ConsoleLogger::info("=== System Information ===");

        std::cout << "  OS Version:      " << m_versionInfo << std::endl;
        std::cout << "  CPU:             " << m_cpuInfo << std::endl;
        std::cout << "  RAM:             " << m_totalRam << " GB" << std::endl;

        std::cout << "  DirectX 12:      ";

        std::cout << std::endl;
    }

    void SystemInfo::printGraphicsAdapters() const
    {
        ConsoleLogger::info("=== Graphics Adapters ===");

        const auto& adapters = m_DX12Adapters;

        if (adapters.empty()) {
            ConsoleLogger::warning("No graphics adapters found!");
            return;
        }

        for (size_t i = 0; i < adapters.size(); ++i) {
            const auto& adapter = adapters[i];

            std::cout << "\n  [" << i << "] " << adapter.name << std::endl;
            std::cout << "      Vendor:          " << m_gpuVendorName
                      << " (0x" << std::hex << adapter.vendorId << std::dec << ")" << std::endl;
            std::cout << "      Device ID:       0x" << std::hex << adapter.deviceId << std::dec << std::endl;
            std::cout << "      Dedicated VRAM:  " << adapter.dedicatedVideoMemory << " MB" << std::endl;
            std::cout << "      System RAM:      " << adapter.dedicatedSystemMemory << " MB" << std::endl;
            std::cout << "      Shared RAM:      " << adapter.sharedSystemMemory << " MB" << std::endl;

            if (!adapter.driverVersion.empty()) {
                std::cout << "      Driver Version:  " << adapter.driverVersion << std::endl;
            }

            std::cout << "      Type:            " << (adapter.isSoftware ? "Software" : "Hardware") << std::endl;

            if (i == 0 && !adapter.isSoftware) {
                ConsoleLogger::success("      >> This adapter will be used by default");
            }
        }

        std::cout << std::endl;
    }
}