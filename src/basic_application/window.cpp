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

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(h_wnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(h_wnd, &ps);
            return 0;
        }

        case WM_CLOSE:
        case WM_QUIT: {
            if (window != nullptr) {
                window->set_should_close(true);
            }
        }

        default: {
            if (window != nullptr) {
                SystemMessage msg;
                msg.window_ptr = window;
                msg.native_message.message = message;
                msg.native_message.wParam = w_param;
                msg.native_message.lParam = l_param;
                window->process_system_message(&msg);
            }
        }
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(h_wnd, message, w_param, l_param);
}

int Window::create(const char* name, Rectangle* rect, WindowSettings* settings) {
    if (settings != nullptr) {
        m_settings = *settings;
    }

    if (rect != nullptr) {
        m_content_rect = *rect;
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

    RECT content_rect;
    GetClientRect(m_handle, &content_rect);
    m_content_rect = {content_rect.left, content_rect.top, content_rect.right, content_rect.bottom};
    m_windowed_rect = {windowed_rect.left, windowed_rect.top, windowed_rect.right, windowed_rect.bottom};

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
    std::cout << "Message " << message->native_message.message << " " << message->native_message.lParam << " "
              << message->native_message.wParam << std::endl;
}

}  // namespace lag
