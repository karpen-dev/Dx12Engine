#include "./core/engine/Engine.hpp"
#include "./core/ConsoleLogger.hpp"
#include "./core/preloader/Preloader.hpp"
#include "./api/player/Player.hpp"

#include <comdef.h>
#include <iostream>
#include <string>
#include <Windows.h>

DX12::Player* g_player = nullptr;

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
    ConsoleLogger::success("║                     DirectX 12 Engine                        ║");
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

    if (!engine.initialize(1280, 720, L"DirectX 12 Engine")) {
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

    DX12::Player player;
    g_player = &player;

    player.setMoveSpeed(8.0f);
    player.setMouseSensitivity(0.003f);
    player.setPosition(DirectX::XMFLOAT3(0.0f, 2.0f, -5.0f));

    engine.setPlayer(player);

    ConsoleLogger::success("Player created and attached to engine!");
    ConsoleLogger::info("  - Move Speed: 8.0 units/sec");
    ConsoleLogger::info("  - Mouse Sensitivity: 0.003");
    ConsoleLogger::info("  - Start Position: (0, 2, -5)");

    ConsoleLogger::success("Engine initialized successfully!");

    ConsoleLogger::info("\n=== Controls ===");
    ConsoleLogger::info("  Movement:     WASD or Arrow Keys");
    ConsoleLogger::info("  Look Around:  Mouse (click to capture)");
    ConsoleLogger::info("  Jump:         Space or E");
    ConsoleLogger::info("  Crouch:       Ctrl or Q");
    ConsoleLogger::info("  Capture Mouse: Left Click or F1");
    ConsoleLogger::info("  Release Mouse: Right Click or ESC");
    ConsoleLogger::info("  Exit:         ESC (when mouse not captured)");
    ConsoleLogger::info("\n=== Starting Main Loop ===");

    engine.run();

    std::cout << "\n";
    ConsoleLogger::info("=== Engine Shutdown ===");

    player.setMouseCapture(false);
    engine.shutdown();

    ConsoleLogger::success("Engine shut down successfully!");

    std::cout << "\n";
    ConsoleLogger::info("Press Enter to exit...");
    std::cin.get();

    g_player = nullptr;

    return 0;
}