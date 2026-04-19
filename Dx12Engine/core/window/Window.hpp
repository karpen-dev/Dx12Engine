#pragma once

#include <Windows.h>
#include "../../api/player/Player.hpp"

namespace DX12 {
	class Player;
	class PlayerController;

	class Window {
	public:
		Window();
		~Window();

		bool initialize(int width, int height, LPCWSTR title);
		void shutdown();

		bool processMessages();
		HWND getHandle() const { return m_hwnd; }
		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }

		void setPlayer(Player* player);
		Player* getPlayer() const;

		void setMouseCapture(bool capture);
		bool isMouseCaptured() const;

		bool isKeyPressed(int key) const;

		void updateInput();

	private:
		static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT handleMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void updateMouseCapture();
		void centerMouse();
		void handleKeyboard(UINT msg, WPARAM wParam);
		void handleMouse(UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_hwnd;
		int m_width;
		int m_height;
		bool m_running;

		Player* m_player;
		PlayerController* m_playerController;

		bool m_keys[256];
		bool m_mouseCaptured;
		int m_centerX;
		int m_centerY;
		bool m_mouseInitialized;

		int m_lastMouseX;
		int m_lastMouseY;
		bool m_firstMouse;
	};

}