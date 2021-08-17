#ifndef LAG_APPLICATION_WINDOWS_HPP
#define LAG_APPLICATION_WINDOWS_HPP

#include <Windows.h>

namespace lag {

using NativeWindowHandle = HWND;

struct WindowHandle {
    NativeWindowHandle window;
};

struct Window {
    WindowHandle handle;
};

struct SystemMessage {
    NativeWindowHandle window;
    UINT message;
    WPARAM w_param;
    LPARAM l_param;
};

struct ApplicationArgs {
    HINSTANCE h_instance;
    HINSTANCE h_pre_instance;
    LPSTR lp_cmd_line;  // ASCII string pointer to command line
    int n_cmd_show;     // show state of window
};

}  // namespace lag

#endif  // LAG_APPLICATION_WINDOWS_HPP
