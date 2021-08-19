#ifndef LAG_APPLICATION_HPP
#define LAG_APPLICATION_HPP

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
    void poll_system_events();

    Window m_window;

    bool m_must_exit = false;

    bool m_need_reaload = false;
};

}  // namespace lag

#endif  // LAG_APPLICATION_HPP