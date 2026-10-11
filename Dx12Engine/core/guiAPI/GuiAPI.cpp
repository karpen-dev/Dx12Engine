//
// Created by chino on 2026/10/11.
//

#include "GuiAPI.hpp"
#include "imgui.h"

#include <cstdarg>
#include <vector>

namespace DX12 {
    bool GuiAPI::beginWindow(const std::string &title, bool *open) {
        return ImGui::Begin(title.c_str(), open);
    }

    void GuiAPI::endWindow() {
        ImGui::End();
    }

    void GuiAPI::text(const std::string textFormat, ...) {
        va_list args;
        va_start(args, textFormat);
        ImGui::TextV(textFormat.c_str(), args);
        va_end(args);
    }

    void GuiAPI::textColored(float r, float g, float b, float a, const std::string textFormat, ...) {
        va_list args;
        va_start(args, textFormat);
        ImGui::TextColoredV(ImVec4(r, g, b, a), textFormat.c_str(), args);
        va_end(args);
    }

    void GuiAPI::separator() {
        ImGui::Separator();
    }

    bool GuiAPI::button(const std::string& label, float width, float height) {
        return ImGui::Button(label.c_str(), ImVec2(width, height));
    }

    bool GuiAPI::checkbox(const std::string& label, bool* value) {
        return ImGui::Checkbox(label.c_str(), value);
    }

    bool GuiAPI::sliderFloat(const std::string& label, float* value, float min, float max) {
        return ImGui::SliderFloat(label.c_str(), value, min, max);
    }

    bool GuiAPI::inputFloat3(const std::string& label, float value[3]) {
        return ImGui::InputFloat3(label.c_str(), value);
    }

    void GuiAPI::plotLines(const std::string& label,
                          const std::vector<float>& values,
                          const std::string& overlayText,
                          float minScale,
                          float maxScale,
                          float height)
    {
        if (values.empty()) return;

        ImGui::PlotLines(
            label.c_str(),
            values.data(),
            static_cast<int>(values.size()),
            0,
            overlayText.empty() ? nullptr : overlayText.c_str(),
            minScale,
            maxScale,
            ImVec2(0, height)
        );
    }
}
