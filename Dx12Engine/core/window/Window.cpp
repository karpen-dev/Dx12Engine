#include "Window.hpp"

#include <stdexcept>
#include <windowsx.h>

#include "core/ConsoleLogger.hpp"

namespace DX12 {

    Window::Window()
        : m_hwnd(nullptr)
        , m_width(800)
        , m_height(600)
        , m_running(true)
        , m_player(nullptr)
        , m_playerController(nullptr)
        , m_mouseCaptured(false)
        , m_centerX(0)
        , m_centerY(0)
        , m_mouseInitialized(false)
        , m_lastMouseX(0)
        , m_lastMouseY(0)
        , m_firstMouse(true) {

        ZeroMemory(m_keys, sizeof(m_keys));
    }

    Window::~Window() {
        shutdown();
        delete m_playerController;
    }

    bool Window::initialize(int width, int height, LPCWSTR title) {
        m_width = width;
        m_height = height;

        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = windowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.lpszClassName = L"DX12EngineWindow";

        if (!RegisterClassExW(&wc)) {
            DWORD error = GetLastError();
            ConsoleLogger::error("RegisterClassExW failed! Error code: " + std::to_string(error));

            return false;
        }

        m_hwnd = CreateWindowExW(
            0,
            L"DX12EngineWindow",
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            m_width, m_height,
            nullptr,
            nullptr,
            wc.hInstance,
            this
        );

        if (!m_hwnd) {
            DWORD error = GetLastError();
            ConsoleLogger::error("CreateWindowExW failed! Error code: " + std::to_string(error));

            return false;
        }

        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);

        return true;
    }

    void Window::shutdown() {
        if (m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
    }

    bool Window::processMessages() {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                m_running = false;
                return false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return m_running;
    }

    void Window::setPlayer(Player* player) {
        m_player = player;

        ConsoleLogger::debug("Setting player to window");

        if (m_player && !m_playerController) {
            m_playerController = new PlayerController(m_player);
            ConsoleLogger::success("PlayerController created successfully");
        } else if (!m_player) {
            ConsoleLogger::error("Player is null in setPlayer");
        }
    }

    Player* Window::getPlayer() const {
        return m_player;
    }

    void Window::setMouseCapture(bool capture) {
        if (m_mouseCaptured == capture) return;

        m_mouseCaptured = capture;
        updateMouseCapture();
    }

    bool Window::isMouseCaptured() const {
        return m_mouseCaptured;
    }

    bool Window::isKeyPressed(int key) const {
        if (key >= 0 && key < 256) {
            return m_keys[key];
        }
        return false;
    }

    void Window::updateInput() {
        if (!m_playerController || !m_player) return;

        m_playerController->handleKeyboard(
            m_keys['W'] || m_keys[VK_UP],
            m_keys['S'] || m_keys[VK_DOWN],
            m_keys['A'] || m_keys[VK_LEFT],
            m_keys['D'] || m_keys[VK_RIGHT],
            m_keys['E'] || m_keys[VK_SPACE],
            m_keys['Q'] || m_keys[VK_CONTROL]
        );
    }

    void Window::updateMouseCapture() {
        if (m_mouseCaptured) {
            SetCapture(m_hwnd);
            ShowCursor(false);

            centerMouse();
            m_mouseInitialized = true;
            m_firstMouse = true;
        } else {
            ReleaseCapture();
            ShowCursor(true);
        }
    }

    void Window::centerMouse() {
        RECT rect;
        GetClientRect(m_hwnd, &rect);

        m_centerX = (rect.left + rect.right) / 2;
        m_centerY = (rect.top + rect.bottom) / 2;

        POINT centerPoint = { m_centerX, m_centerY };
        ClientToScreen(m_hwnd, &centerPoint);

        SetCursorPos(centerPoint.x, centerPoint.y);

        m_lastMouseX = m_centerX;
        m_lastMouseY = m_centerY;
    }

    void Window::handleKeyboard(UINT msg, WPARAM wParam) {
        switch (msg) {
            case WM_KEYDOWN:
                if (wParam < 256) {
                    m_keys[wParam] = true;

                    if (wParam == VK_ESCAPE) {
                        if (m_mouseCaptured) {
                            setMouseCapture(false);
                        } else {
                            PostQuitMessage(0);
                        }
                    }

                    if (wParam == VK_F1) {
                        setMouseCapture(!m_mouseCaptured);
                    }
                }
                break;

            case WM_KEYUP:
                if (wParam < 256) {
                    m_keys[wParam] = false;
                }
                break;
        }
    }

    void Window::handleMouse(UINT msg, WPARAM wParam, LPARAM lParam) {
        if (!m_playerController) return;

        switch (msg) {
            case WM_LBUTTONDOWN:
                if (!m_mouseCaptured) {
                    setMouseCapture(true);
                }
                break;

            case WM_RBUTTONDOWN:
                if (m_mouseCaptured) {
                    setMouseCapture(false);
                }
                break;

            case WM_MOUSEMOVE:
                if (m_mouseCaptured && m_mouseInitialized) {
                    int mouseX = GET_X_LPARAM(lParam);
                    int mouseY = GET_Y_LPARAM(lParam);

                    if (m_firstMouse) {
                        m_lastMouseX = mouseX;
                        m_lastMouseY = mouseY;
                        m_firstMouse = false;
                        break;
                    }

                    int deltaX = mouseX - m_lastMouseX;
                    int deltaY = mouseY - m_lastMouseY;

                    if (deltaX != 0 || deltaY != 0) {
                        m_playerController->handleMouse((float)deltaX, (float)deltaY);
                    }

                    centerMouse();
                }
                break;
        }
    }

    LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* window = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = static_cast<Window*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        } else {
            window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (window) {
            return window->handleMessages(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT Window::handleMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
        case WM_KEYUP:
            handleKeyboard(msg, wParam);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MOUSEMOVE:
            handleMouse(msg, wParam, lParam);
            return 0;

        case WM_SIZE:
            m_width = LOWORD(lParam);
            m_height = HIWORD(lParam);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

}
