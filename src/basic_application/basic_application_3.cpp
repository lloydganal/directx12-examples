

#include "window.hpp"

namespace lag {

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

    virtual const char* get_name() const = 0;

  protected:
    Window* get_window() { return &m_window; }

  private:
    Window m_window;

    bool m_must_exit = false;

    bool m_need_reaload = false;
};

int ApplicationInterface::run() {
    {
        Rectangle rect = {0, 0, 800, 800};
        m_window.set_rectangle(rect);
    }

    if (on_setup() == RV_ERROR) {
        return RV_ERROR;
    }

    m_window.create("My App", nullptr, nullptr);
    m_window.show();

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

        MSG msg;
        msg.message = NULL;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (m_window.should_close()) {
            m_must_exit = true;
        }
    }

    m_window.destroy();

    on_exit();

    return RV_SUCCESS;
}

}  // namespace lag

class Application : public lag::ApplicationInterface {
  public:
    int on_setup() {
        lag::Rectangle rect = {0, 0, 1920, 1080};
        get_window()->set_rectangle(rect);
        return RV_SUCCESS;
    }

    int on_initialize() { return RV_SUCCESS; }

    void on_exit() {}

    void on_update() {}

    void on_draw() {}

    int on_load() { return RV_SUCCESS; }

    void on_unload() {}

    const char* get_name() const { return "Basic Application 2"; }
};

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE, LPSTR, int n_cmd_show) {
    Application app;
    return app.run();
}
