#include "application_windows.hpp"

#include "application.hpp"

namespace lag {
LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    ApplicationInterface* app = reinterpret_cast<ApplicationInterface*>(GetWindowLongPtr(h_wnd, GWLP_USERDATA));

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
                app->on_input_message();
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
}  // namespace lag
