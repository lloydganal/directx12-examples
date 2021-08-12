#include <windows.h>

#include <iostream>

class Example {
  public:
    virtual int on_initialize() { return 0; }
    virtual int on_exit() { return 0; }
    virtual int on_update() { return 0; }
    virtual int on_render() { return 0; }
    virtual int on_key_down(UINT8) {
        std::cout << "Hello" << std::endl;
        return 0;
    }
    virtual int on_key_up(UINT8) { return 0; }
};

struct OSWindow {
    HWND handle;
};

class Application {
  public:
    int run(Example& example, HINSTANCE h_instance, int n_cmd_show) {
        WNDCLASSEX window_class = {0};
        window_class.cbSize = sizeof(WNDCLASSEX);
        window_class.style = CS_HREDRAW | CS_VREDRAW;
        window_class.lpfnWndProc = Application::window_proc;
        window_class.hInstance = h_instance;
        window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
        window_class.lpszClassName = "ExampleApplication";
        RegisterClassEx(&window_class);

        RECT window_rect = {0, 0, static_cast<LONG>(800), static_cast<LONG>(600)};
        AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

        m_os_window.handle = CreateWindow(window_class.lpszClassName,            //
                              "Example Application",                 //
                              WS_OVERLAPPEDWINDOW,                   //
                              CW_USEDEFAULT,                         //
                              CW_USEDEFAULT,                         //
                              window_rect.right - window_rect.left,  //
                              window_rect.bottom - window_rect.top,  //
                              nullptr,                               // We have no parent window.
                              nullptr,                               // We aren't using menus.
                              h_instance,                            //
                              &example);                             //

        example.on_initialize();

        ShowWindow(m_os_window.handle, n_cmd_show);

        MSG msg = {};
        while (msg.message != WM_QUIT) {
            // Process any messages in the queue.
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        example.on_exit();

        return 0;
    }

    OSWindow* get_os_window() { return &m_os_window; }

  protected:
    static LRESULT CALLBACK window_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        Example* pSample = reinterpret_cast<Example*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        switch (message) {
            case WM_CREATE: {
                // Save the DXSample* passed in to CreateWindow.
                LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
                SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
                return 0;
            }

            case WM_KEYDOWN: {
                if (pSample) {
                    pSample->on_key_down(static_cast<UINT8>(wParam));
                }
                return 0;
            }

            case WM_KEYUP: {
                if (pSample) {
                    pSample->on_key_up(static_cast<UINT8>(wParam));
                }
                return 0;
            }

            case WM_PAINT: {
                if (pSample) {
                    pSample->on_update();
                    pSample->on_render();
                }
                return 0;
            }

            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }

            default : {

            }
        }

        // Handle any messages the switch statement didn't.
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

  private:
    OSWindow m_os_window;
};

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE, LPSTR, int n_cmd_show) {
    Application app;
    Example example;
    return app.run(example, h_instance, n_cmd_show);
}
