#pragma once

#include "../Types.hpp"
#include <Windows.h>

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

	private:
		static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_hwnd;
		int m_width;
		int m_height;
		std::wstring m_title;
	};
}