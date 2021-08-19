#ifndef LAG_WINDOW_HPP
#define LAG_WINDOW_HPP

#include "os.hpp"

namespace lag {

#if defined(LAG_TARGET_OS_WINDOWS)
typedef HWND NativeWindowHandle;
typedef tagMSG NativeWindowMessage;
#endif

struct NativeWindow {
    NativeWindowHandle handle;
};

class Window;

struct SystemMessage {
#if defined(LAG_TARGET_OS_WINDOWS)
    Window* window_ptr;
    NativeWindowMessage native_message;
#endif
};

struct WindowSettings {
    bool monitor = false;
    bool decorated = true;
    bool resizable = true;
    bool centered = true;
};

class InputHandler;

class Window {
  public:
    Window();

    ~Window();

    int create(const char* name, Rectangle* rect = nullptr, WindowSettings* settings = nullptr);

    void destroy();

    int show();

    constexpr void set_input_handler(InputHandler* input_handler_ptr) { m_input_handler_ptr = input_handler_ptr; }

    constexpr void set_settings(WindowSettings& settings) {
        if (!m_window_created) {  // This shall be an assert
            m_settings = settings;
        }
    }

    constexpr void set_rectangle(Rectangle& rect) {
        if (!m_window_created) {  // This shall be an assert
            m_content_rect = rect;
        }
    }

    constexpr void set_should_close(bool should_close) { m_should_close = should_close; }

    constexpr bool should_close() const { return m_should_close; }

    constexpr bool window_created() const { return m_window_created; }

    void process_system_message(SystemMessage* message);

  private:
    NativeWindowHandle m_handle;

    WindowSettings m_settings;

    Rectangle m_windowed_rect;
    Rectangle m_content_rect;
    Rectangle m_fullscreen_rect;

    InputHandler* m_input_handler_ptr;

    bool m_window_created = false;

    bool m_should_close = false;
};

}  // namespace lag

#endif  // LAG_WINDOW_HPP