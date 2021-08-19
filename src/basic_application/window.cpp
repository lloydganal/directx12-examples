#include "window.hpp"

#include <iostream>

namespace lag {

Window::Window() {}

Window::~Window() {}

static LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(h_wnd, GWLP_USERDATA));

    switch (message) {
        case WM_CREATE: {
            // Save the DXSample* passed in to CreateWindow.
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(l_param);
            SetWindowLongPtr(h_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            return 0;
        }

        default: {
            if (window != nullptr) {
                SystemMessage msg;
                msg.hwnd = h_wnd;
                msg.message = message;
                msg.wParam = w_param;
                msg.lParam = l_param;
                window->process_system_message(&msg);
            }
        }
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(h_wnd, message, w_param, l_param);
}

int Window::create(const char* name, Rectangle* rect_ptr, WindowSettings* settings) {
    if (settings != nullptr) {
        m_settings = *settings;
    }

    if (rect_ptr != nullptr) {
        m_content_rect = *rect_ptr;
    }

    long window_style = 0 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    if (m_settings.monitor)
        window_style |= WS_POPUP;
    else {
        window_style |= WS_SYSMENU | WS_MINIMIZEBOX;
        if (m_settings.decorated) {
            window_style |= WS_CAPTION | WS_SIZEBOX;
            if (m_settings.resizable) {
                window_style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
            }
        } else {
            window_style |= WS_POPUP;
        }
    }

    RECT windowed_rect = {m_content_rect.x, m_content_rect.y, m_content_rect.width, m_content_rect.height};

    if (m_settings.centered) {
        windowed_rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - windowed_rect.right / 2;
        windowed_rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - windowed_rect.bottom / 2;
        windowed_rect.right += windowed_rect.left;
        windowed_rect.bottom += windowed_rect.top;
    }

    AdjustWindowRect(&windowed_rect, window_style, FALSE);

    HINSTANCE h_instance = (HINSTANCE)GetModuleHandle(NULL);

    WNDCLASSA window_class = {};
    window_class.lpfnWndProc = (WNDPROC)window_proc;
    window_class.hInstance = h_instance;
    window_class.lpszClassName = name;
    RegisterClassA(&window_class);

    // https://docs.microsoft.com/en-us/previous-versions/ms960010(v=msdn.10)
    m_handle = CreateWindowExA(
        // Optional window styles
        0,
        // Window class name
        window_class.lpszClassName,
        // Window title
        name,
        // Window style
        window_style,
        // Window position origin x
        windowed_rect.left,
        // Window position origin y
        windowed_rect.top,
        // Width
        windowed_rect.right - windowed_rect.left,
        // Height
        windowed_rect.bottom - windowed_rect.top,
        // Parent
        nullptr,
        // Menu
        nullptr,
        // Instance handle
        h_instance,
        // Additional application data
        this);

    if (m_handle == nullptr) {
        return RV_ERROR;
    }

    RECT rect;
    GetClientRect(m_handle, &rect);
    m_content_rect = {rect.left, rect.top, rect.right, rect.bottom};

    GetWindowRect(m_handle, &rect);
    m_windowed_rect = {rect.left, rect.top, rect.right, rect.bottom};

    return 0;
}

void Window::destroy() {
    if (m_handle != nullptr) {
        DestroyWindow(m_handle);
        m_handle = nullptr;
    }
}

int Window::show() { return ShowWindow(m_handle, SW_SHOW); }

void Window::process_system_message(SystemMessage* message) {
    switch (message->message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(message->hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(message->hwnd, &ps);
        } break;

        case WM_CLOSE:
        case WM_QUIT: {
            set_should_close(true);
        } break;

        case WM_MOVE: {
            // to do - handle fullscreen
            RECT rect;
            GetWindowRect(m_handle, &rect);
            m_windowed_rect = {rect.left, rect.top, rect.right, rect.bottom};

        } break;

        case WM_SIZE: {
            switch (message->wParam) {
                case SIZE_RESTORED:
                case SIZE_MAXIMIZED:
                    m_is_minimized = false;
                    if (/*!gWindow->fullScreen && */ !m_is_resizing) {
                        RECT rect;
                        GetClientRect(m_handle, &rect);
                        m_content_rect = {rect.left, rect.top, rect.right, rect.bottom};
                        // onResize(gWindow, getRectWidth(gWindow->clientRect), getRectHeight(gWindow->clientRect));
                        RedrawWindow(m_handle, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);
                    }
                    break;

                case SIZE_MINIMIZED:
                    m_is_minimized = true;
                    break;

                default:
                    break;
            }
        } break;

        case WM_SETFOCUS: {
            m_is_focused = true;
        } break;

        case WM_KILLFOCUS: {
            m_is_focused = false;
        } break;

        case WM_ENTERSIZEMOVE: {
            m_is_resizing = true;

        } break;

        case WM_EXITSIZEMOVE: {
            m_is_resizing = false;
            // if (!gWindow->fullScreen) {
            RECT rect;
            GetClientRect(m_handle, &rect);
            m_content_rect = {rect.left, rect.top, rect.right, rect.bottom};
            // onResize(gWindow, getRectWidth(gWindow->clientRect), getRectHeight(gWindow->clientRect));
            //}

        } break;

        case WM_SIZING: {
        } break;

        default: {
        }
    }
}

}  // namespace lag
