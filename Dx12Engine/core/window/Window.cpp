#include "Window.hpp"

namespace DX12 {
	Window::Window(const int width, const int height, const std::wstring& title)
		: m_hwnd(nullptr), m_width(width), m_height(height), m_title(title) {}

	Window::~Window()
	{
		shutdown();
	}

	bool Window::init()
	{
		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = windowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = L"DX12WindowClass";

		if (!RegisterClassExW(&wc))
			return false;

		RECT rect = {0, 0, m_width, m_height};
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		m_hwnd = CreateWindowExW(
			0,
			L"DX12WindowClass",
			m_title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr, nullptr,
			wc.hInstance,
			this
		);

		if (!m_hwnd)
			return false;

		ShowWindow(m_hwnd, SW_SHOW);

		return true;
	}

	void Window::shutdown()
	{
		if (m_hwnd)
		{
			DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
		}
	}

	bool Window::proccessMessages()
	{
		MSG msg = {};

		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return false;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return true;
	}

	LRESULT Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_NCCREATE)
		{
			LPCREATESTRUCTW cs = reinterpret_cast<LPCREATESTRUCTW>(lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		}

		Window* w = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch (msg)
		{
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;

			case WM_KEYDOWN:
			{
				if (wParam == VK_ESCAPE) {
					PostQuitMessage(0);
					return 0;
				}
				if (w) w->onKeyDown(wParam);
				return 0;
			}

			case WM_KEYUP:
			{
				if (w) w->onKeyUp(wParam);
				return 0;
			}
		}

		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}
