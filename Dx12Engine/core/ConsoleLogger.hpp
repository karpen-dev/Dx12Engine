#pragma once

#include <iostream>
#include <string>
#include <windows.h>

class ConsoleLogger {
public:
    enum class Color {
        Default = 7,
        Red = 12,
        Green = 10,
        Yellow = 14,
        Cyan = 11,
        White = 15
    };

    static void setColor(Color color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    }

    static void resetColor() {
        setColor(Color::Default);
    }

    static void info(const std::string& message) {
        setColor(Color::Cyan);
        std::cout << "[INFO] ";
        resetColor();
        std::cout << message << std::endl;
    }

    static void success(const std::string& message) {
        setColor(Color::Green);
        std::cout << "[SUCCESS] ";
        resetColor();
        std::cout << message << std::endl;
    }

    static void warning(const std::string& message) {
        setColor(Color::Yellow);
        std::cout << "[WARNING] ";
        resetColor();
        std::cout << message << std::endl;
    }

    static void error(const std::string& message) {
        setColor(Color::Red);
        std::cout << "[ERROR] ";
        resetColor();
        std::cout << message << std::endl;
    }

    static void debug(const std::string& message) {
#ifdef _DEBUG
        setColor(Color::White);
        std::cout << "[DEBUG] ";
        resetColor();
        std::cout << message << std::endl;
#endif
    }
};