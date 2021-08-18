// directx12-examples/basic_application_1.cpp
// by Lloyd Anthony Ganal (11/08/2021)
//
// Copyright 2021 Lloyd Anthony Ganal.
// Use of this source code is governed by the WTFPL
// license that can be found in the LICENSE file.

// ===========================================================================
// TO DO
// ---------------------------------------------------------------------------
// - Complete window handling
// ===========================================================================

#include <Windows.h>

#include <iostream>

#define RV_SUCCESS 0
#define RV_MUST_EXIT 1
#define RV_ERROR -1

namespace lag {
// ===========================================================================
// application_internal.hpp
// ---------------------------------------------------------------------------

struct Rectangle {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

struct Resolution {
    uint32_t width;
    uint32_t height;
};

#if defined(_WIN32)
typedef HWND NativeWindowHandle;
#endif

struct WindowHandle {
    NativeWindowHandle window;
};

struct Window {
    WindowHandle handle;
    Rectangle windowed_rect;
    // Rectangle fullscreen_rect;  // ??
    Rectangle client_rect;
    // uint32_t windows_flags;
    // bool full_screen;
    // bool cursor_tracked;
    // bool iconified;
    // bool maximized;
    // bool minimized;
    // bool hide;
    // bool noresize_frame;
    // bool borderless_window;
    // bool override_default_position;
    // bool centered;
    // bool force_low_dpi;

    bool monitor = false;
    bool decorated = true;
    bool resizable = true;
    bool centered = true;
};

struct SystemMessage {
#if defined(_WIN32)
    Window* window;
    UINT message;
    WPARAM w_param;
    LPARAM l_param;
#endif
};

// ===========================================================================
// application.hpp
// ---------------------------------------------------------------------------

class ApplicationInterface {
  public:
    int run();

    // Any set up tasks before initialization (window settings, etc.).
    virtual int on_setup() = 0;

    // Load subsystem configurations, application lifetime data, subsystem initializations, etc.
    virtual int on_initialize() = 0;

    // cleanup
    virtual void on_exit() = 0;

    // Update scene data, etc.
    virtual void on_update() = 0;

    // Perform the rendering and presentation to the main window.
    virtual void on_draw() = 0;

    // Load scene content
    virtual int on_load() = 0;

    // unload scene content
    virtual void on_unload() = 0;

    // Let theuser code handle the system input messages
    virtual void on_system_message(SystemMessage* message) = 0;

    virtual const char* get_name() const = 0;

  protected:
    Window* get_window() { return &m_window; }

  private:
    int poll_system_events();

    int create_window();

    Window m_window = {};

    bool m_running = false;

    bool m_must_exit = false;

    bool m_need_reaload = false;
};

// ===========================================================================
// application_internal.cpp
// ---------------------------------------------------------------------------
static ApplicationInterface* g_application = nullptr;

#if defined(_WIN32)
LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param) {
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

        default: {
            if (g_application && window) {
                SystemMessage msg;
                msg.window = window;
                msg.message = message;
                msg.w_param = w_param;
                msg.l_param = l_param;
                g_application->on_system_message(&msg);
            }
        }
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(h_wnd, message, w_param, l_param);
}
#endif

int ApplicationInterface::run() {
    m_running = true;
    g_application = this;

    m_window.client_rect = {0, 0, 800, 600};

    if (on_setup() == RV_ERROR) {
        return RV_ERROR;
    }

    if (create_window() == RV_ERROR) {
        return RV_ERROR;
    }

    if (on_initialize() == RV_ERROR) {
        return RV_ERROR;
    }

    if (on_load() == RV_ERROR) {
        return RV_ERROR;
    }

    while (!m_must_exit) {
        if (m_need_reaload) {
            on_unload();
            on_load();
        }

        on_update();
        on_draw();
        poll_system_events();
    }

    on_exit();

    return RV_SUCCESS;
}

int ApplicationInterface::poll_system_events() {
    MSG msg;
    msg.message = NULL;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (WM_CLOSE == msg.message || WM_QUIT == msg.message) {
            m_must_exit = true;
        }
    }

    return RV_SUCCESS;
};

int ApplicationInterface::create_window() {
    // Define the window style
    long window_style = 0 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    if (m_window.monitor)
        window_style |= WS_POPUP;
    else {
        window_style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (m_window.decorated) {
            window_style |= WS_CAPTION | WS_SIZEBOX;
            if (m_window.resizable) {
                window_style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
            }
        } else {
            window_style |= WS_POPUP;
        }
    }

    RECT window_rect = {m_window.client_rect.x,        // left
                        m_window.client_rect.y,        // top
                        m_window.client_rect.width,    // right
                        m_window.client_rect.height};  // bottom

    if (m_window.centered) {
        window_rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - window_rect.right / 2;
        window_rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - window_rect.bottom / 2;
        window_rect.right += window_rect.left;
        window_rect.bottom += window_rect.top;
    }

    AdjustWindowRect(&window_rect, window_style, FALSE);

    HINSTANCE h_instance = (HINSTANCE)GetModuleHandle(NULL);

    WNDCLASSEX window_class = {0};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = h_instance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = "My";
    RegisterClassEx(&window_class);

    m_window.handle.window = CreateWindow(
        //
        window_class.lpszClassName,
        //
        "My",
        //
        window_style,
        //
        window_rect.left,
        //
        window_rect.top,
        //
        window_rect.right - window_rect.left,
        //
        window_rect.bottom - window_rect.top,
        // We have no parent window.
        nullptr,
        // We aren't using menus.
        nullptr,
        //
        h_instance,
        //
        this);

    if (!m_window.handle.window) {
        return RV_ERROR;
    }

    ShowWindow(m_window.handle.window, SW_SHOW);

    RECT area;
    GetClientRect(m_window.handle.window, &area);

    m_window.windowed_rect = {window_rect.left, window_rect.top, window_rect.right, window_rect.bottom};

    return RV_SUCCESS;
}

}  // namespace lag

class Application : public lag::ApplicationInterface {
  public:
    int on_setup() {
        lag::Window* p_window = get_window();

        p_window->client_rect.width = 1920;
        p_window->client_rect.height = 1080;

        return RV_SUCCESS;
    }

    int on_initialize() { return RV_SUCCESS; }

    void on_exit() {}

    void on_update() {}

    void on_draw() {}

    int on_load() { return RV_SUCCESS; }

    void on_unload() {}

    void on_system_message(lag::SystemMessage* message) {
        std::cout << "Message " << message->message << " " << message->l_param << " " << message->w_param << std::endl;
    }

    const char* get_name() const { return "Basic Application 2"; }
};

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE, LPSTR, int n_cmd_show) {
    Application app;
    return app.run();
}
