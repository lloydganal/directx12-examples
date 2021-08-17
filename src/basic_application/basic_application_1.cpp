// directx12-examples/basic_application_1.cpp
// by Lloyd Anthony Ganal (11/08/2021)
//
// Copyright 2021 Lloyd Anthony Ganal.
// Use of this source code is governed by the WTFPL
// license that can be found in the LICENSE file.

#include <Windows.h>

#include <cstdint>
#include <iostream>

#define RV_SUCCESS 0
#define RV_ERROR -1

using WindowHandle = HWND;

struct Window {
    WindowHandle handle = nullptr;

    struct Settings {
        int32_t width = 800;
        int32_t height = 600;
        const char* title;
    } settings;
};

class ApplicationBase {
  public:
    int run();

  protected:
    virtual int on_setup() = 0;

    virtual int on_initialize() = 0;

    virtual int on_exit() = 0;

    virtual int on_update() = 0;

    virtual int on_draw() = 0;

    virtual int on_load() = 0;

    virtual int on_unload() = 0;

    virtual int on_input_event() {
        std::cout << "Hello" << std::endl;
        return 0;
    }

    Window* get_window() { return &m_window; }

  private:
    Window m_window;

    int create_window();

    static LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param);

    bool m_running = false;
};

int ApplicationBase::run() {
    if (m_running) {
        // Error here. Only one call to run allowed
    } else {
        m_running = true;
    }

    if (create_window() != RV_SUCCESS) {
        return RV_ERROR;
    }

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return RV_SUCCESS;
}

int ApplicationBase::create_window() {
    HINSTANCE h_instance = (HINSTANCE)GetModuleHandle(NULL);

    WNDCLASSEX window_class = {0};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = ApplicationBase::window_proc;
    window_class.hInstance = h_instance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = "My";
    RegisterClassEx(&window_class);

    RECT window_rect = {0, 0, static_cast<LONG>(800), static_cast<LONG>(600)};

    window_rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - window_rect.right / 2;
    window_rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - window_rect.bottom / 2;
    window_rect.right += window_rect.left;
    window_rect.bottom += window_rect.top;

    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_window.handle = CreateWindow(            //
        window_class.lpszClassName,            //
        "My",                                  //
        WS_OVERLAPPEDWINDOW,                   //
        window_rect.left,                         //
        window_rect.top,                         //
        window_rect.right - window_rect.left,  //
        window_rect.bottom - window_rect.top,  //
        nullptr,                               // We have no parent window.
        nullptr,                               // We aren't using menus.
        h_instance,                            //
        this);                                 //

    ShowWindow(m_window.handle, SW_SHOW);

    RECT area;
    GetClientRect(m_window.handle, &area);

    return RV_SUCCESS;
}

LRESULT CALLBACK ApplicationBase::window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    ApplicationBase* app = reinterpret_cast<ApplicationBase*>(GetWindowLongPtr(h_wnd, GWLP_USERDATA));

    switch (message) {
        case WM_CREATE: {
            // Save the DXSample* passed in to CreateWindow.
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(l_param);
            SetWindowLongPtr(h_wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
            return 0;
        }

        case WM_KEYDOWN:
        case WM_KEYUP: {
            if (app) {
                app->on_input_event();
            }
            return 0;
        }

        case WM_PAINT: {
            if (app) {
                app->on_update();
                app->on_draw();
            }
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        default: {
            
        }
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(h_wnd, message, w_param, l_param);
}

class Application : public ApplicationBase {
  public:
    int on_setup() override { return RV_SUCCESS; }

    int on_initialize() override { return RV_SUCCESS; }

    int on_exit() override { return RV_SUCCESS; }

    int on_update() override { return RV_SUCCESS; }

    int on_draw() override { return RV_SUCCESS; }

    int on_load() override { return RV_SUCCESS; }

    int on_unload() override { return RV_SUCCESS; }
};

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE, LPSTR, int n_cmd_show) {
    Application app;
    return app.run();
}
