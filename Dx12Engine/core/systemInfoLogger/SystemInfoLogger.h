#pragma once

#include <string>

#include "../camera/Camera.hpp"

namespace DX12
{
    struct AdapterInfo {
        std::string name;
        std::string description;
        uint64_t dedicatedVideoMemory;
        uint64_t dedicatedSystemMemory;
        uint64_t sharedSystemMemory;
        bool isSoftware;
        uint32_t vendorId;
        uint32_t deviceId;
        std::string driverVersion;
    };

    class SystemInfo
    {
    public:
        SystemInfo();

        bool init(uint32_t vendorId);

        void printSystemInfo() const;
        void printGraphicsAdapters() const;
    private:
        std::string m_versionInfo;
        std::string m_cpuInfo;
        std::string m_gpuVendorName;

        unsigned long m_totalRam;

        std::vector<AdapterInfo> m_DX12Adapters;
    };
}