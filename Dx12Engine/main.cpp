#include "./core/engine/Engine.hpp"
#include "./core/ConsoleLogger.hpp"
#include "./core/preloader/Preloader.hpp"

#include <comdef.h>
#include <iostream>
#include <string>
#include <Windows.h>

LONG WINAPI unhandledExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
    ConsoleLogger::error("Unhandled exception occurred!");

    switch (exceptionInfo->ExceptionRecord->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
            ConsoleLogger::error("  Access violation (memory read/write error)");
            ConsoleLogger::error("  Address: 0x" +
                std::to_string(reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress)));
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            ConsoleLogger::error("  Illegal instruction");
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            ConsoleLogger::error("  Integer division by zero");
            break;
        default:
            ConsoleLogger::error("  Exception code: 0x" +
                std::to_string(exceptionInfo->ExceptionRecord->ExceptionCode));
            break;
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

int main() {
    AllocConsole();

    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);

    std::cout << "=== DirectX 12 Engine Starting ===" << std::endl;
    std::cout << "Press Enter to continue..." << std::endl;
    std::cin.get();

    SetUnhandledExceptionFilter(unhandledExceptionHandler);

    DX12::Preloader::setupConsole();

    std::cout << "\n";
    ConsoleLogger::success("╔══════════════════════════════════════════════════════════════╗");
    ConsoleLogger::success("║         DirectX 12 Engine - Triangle Demo v1.0              ║");
    ConsoleLogger::success("╚══════════════════════════════════════════════════════════════╝");
    std::cout << "\n";

    DX12::Preloader::checkDebugLayer();

    if (!DX12::Preloader::testDirectX12Creation()) {
        ConsoleLogger::error("\nDirectX 12 is not supported on this system!");
        ConsoleLogger::info("Requirements:");
        ConsoleLogger::info("  - Windows 10 version 1709 or later");
        ConsoleLogger::info("  - Graphics card with DirectX 12 support");
        ConsoleLogger::info("  - WDDM 2.0 or later driver");

        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    ConsoleLogger::info("\n=== Initializing Engine ===");

    DX12::Engine engine;

    if (!engine.initialize(1024, 768, L"DirectX 12 Triangle Engine")) {
        ConsoleLogger::error("Failed to initialize engine!");

        ConsoleLogger::info("\nTroubleshooting tips:");
        ConsoleLogger::info("  1. Make sure shader files exist in ../shaders/ directory");
        ConsoleLogger::info("  2. Check if your GPU supports DirectX 12 Feature Level 12.0");
        ConsoleLogger::info("  3. Update your graphics drivers to the latest version");
        ConsoleLogger::info("  4. Enable Graphics Tools in Windows Features for debug layer");

        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    ConsoleLogger::success("Engine initialized successfully!");
    ConsoleLogger::info("\n=== Starting Main Loop ===");
    ConsoleLogger::info("  - Triangle is rotating around Y axis");
    ConsoleLogger::info("  - Press ESC to exit");
    ConsoleLogger::info("  - Close window to exit");
    std::cout << "\n";

    engine.run();

    std::cout << "\n";
    ConsoleLogger::info("=== Engine Shutdown ===");
    engine.shutdown();
    ConsoleLogger::success("Engine shut down successfully!");

    std::cout << "\n";
    ConsoleLogger::info("Press Enter to exit...");
    std::cin.get();

    return 0;
}