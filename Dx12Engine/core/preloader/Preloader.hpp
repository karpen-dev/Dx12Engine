#pragma once

namespace DX12
{
    class Preloader
    {
    public:
        static void checkDebugLayer();
        static bool testDirectX12Creation();
        static void setupConsole();
    };
}