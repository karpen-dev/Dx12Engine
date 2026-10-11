//
// Created by chino on 2026/10/11.
//

#pragma once

#ifndef DX12ENGINE_GUIAPI_H
#define DX12ENGINE_GUIAPI_H

#ifdef DX12_ENGINE_EXPORTS
    #define DX12_ENGINE_API __declspec(dllexport)
#else
    #define DX12_ENGINE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>

namespace DX12 {

    class GuiAPI {
    public:
        static bool beginWindow(const std::string& title, bool* open = nullptr);
        static void endWindow();

        static void text(std::string textFormat, ...);
        static void separator();
        static bool button(const std::string& label, float width = 0.0f, float height = 0.0f);
        static bool checkbox(const std::string& label, bool* value);
        static bool sliderFloat(const std::string& label, float* value, float min, float max);
        static bool inputFloat3(const std::string& label, float value[3]);

        static void textColored(float r, float g, float b, float a, std::string textFormat, ...);

        static void plotLines(const std::string& label,
                              const std::vector<float>& values,
                              const std::string& overlayText = "",
                              float minScale = 0.0f,
                              float maxScale = 0.0f,
                              float height = 80.0f);
    };
}

#endif //DX12ENGINE_GUIAPI_H
