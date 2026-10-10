#include "Window.hpp"
#include <windowsx.h>

#include "core/ConsoleLogger.hpp"

namespace DX12 {
	Window::Window(const int width, const int height, const std::wstring& title)
		: m_hwnd(nullptr), m_width(width), m_height(height), m_title(title) {}

	Window::~Window()
	{
		shutdown();
	}

	void Window::captureMouse(bool enabled) {
		if (enabled == m_mouse.captured) return;
		m_mouse.captured = enabled;

		if (enabled) {
			ShowCursor(FALSE);
			SetCapture(m_hwnd);

			RECT r;
			GetClientRect(m_hwnd, &r);

			POINT tl{ r.left, r.top};
			POINT br{ r.right, r.bottom};

			ClientToScreen(m_hwnd, &tl);
			ClientToScreen(m_hwnd, &br);

			RECT clip{ tl.x, tl.y, br.x, br.y};
			ClipCursor(&clip);

			recenterCursor();
		} else {
			ShowCursor(TRUE);
			ReleaseCapture();
			ClipCursor(nullptr);
		}
	}

	void Window::recenterCursor() {
		RECT r;

		GetClientRect(m_hwnd, &r);
		POINT c{ (r.left + r.right) / 2, (r.top + r.bottom) / 2};

		ClientToScreen(m_hwnd, &c);
		SetCursorPos(c.x, c.y);

		m_center = c;
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

			case WM_MOUSEMOVE: {
				if (!w) break;

				if (!w) break;
				if (w->m_mouse.captured) {
					POINT p{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					ClientToScreen(hwnd, &p);
					w->m_mouse.dx += static_cast<float>(p.x - w->m_center.x);
					w->m_mouse.dy += static_cast<float>(p.y - w->m_center.y);
					w->recenterCursor();
				} else {
					w->m_mouse.x = static_cast<float>(GET_X_LPARAM(lParam));
					w->m_mouse.y = static_cast<float>(GET_Y_LPARAM(lParam));
				}

				return 0;
			}

			case WM_LBUTTONDOWN: {
				if (w && !w->getMouse().captured) {
					w->captureMouse(true);
					ConsoleLogger::info("try to capture");
				}

				return 0;
			}
		}

		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}
