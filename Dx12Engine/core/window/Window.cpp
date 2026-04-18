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
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = windowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = reinterpret_cast<LPCSTR>("DX12WindowClass");

		RegisterClassEx(&wc);

		RECT rect = {0, 0, m_width, m_height};
		AdjustWindowRect(&rect, WS_OVERLAPPED, FALSE);

		m_hwnd = CreateWindowEx(
			0,
			"DX12WindowClass",
			reinterpret_cast<LPCSTR>(m_title.c_str()),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr, nullptr,
			wc.hInstance, this
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
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return false;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

	LRESULT Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}