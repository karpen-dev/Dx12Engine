#pragma once

#include <Windows.h>
#include <array>
#include "../Types.hpp"

namespace DX12 {
	class Window {
	public:
		Window(int width, int height, const std::wstring& title);
		~Window();

		bool init();
		void shutdown();
		bool proccessMessages();

		HWND getHandle() const { return m_hwnd; }
		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }
		
		bool isKeyDown(int vk) const noexcept {
			return vk >= 0 && vk < 256 && m_keys[vk];
		}
		bool wasKeyPressed(int vk) const noexcept {
			return vk >= 0 && vk < 256 && m_keysPressed[vk];
		}
		bool wasKeyReleased(int vk) const noexcept {
			return vk >= 0 && vk < 256 && m_keysReleased[vk];
		}

		void endFrame() noexcept {
			m_keysPressed.fill(false);
			m_keysReleased.fill(false);
		}

		void onKeyDown(WPARAM vk) noexcept {
			if (!m_keys[vk]) m_keysPressed[vk] = true;
			m_keys[vk] = true;
		}
		void onKeyUp(WPARAM vk) noexcept {
			if (m_keys[vk]) m_keysReleased[vk] = true;
			m_keys[vk] = false;
		}

	private:
		static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_hwnd;
		int m_width;
		int m_height;
		std::wstring m_title;

		std::array<bool, 256> m_keys{};
		std::array<bool, 256> m_keysPressed{};
		std::array<bool, 256> m_keysReleased{};
	};
}