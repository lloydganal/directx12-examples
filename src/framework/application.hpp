#ifndef LAG_APPLICATION_HPP
#define LAG_APPLICATION_HPP

#include "application_windows.hpp"

namespace lag {

struct ApplicationArgs;

class WiondowInterface;

class ApplicationInterface {
  public:
    virtual int on_initialization() = 0;

    virtual int on_exit() = 0;

    virtual int on_update() = 0;

    virtual int on_draw() = 0;

    virtual int on_load() = 0;

    virtual int on_upload() = 0;

    virtual int on_input_message() = 0;

    int run(ApplicationArgs& args);

  protected:
    constexpr Window* get_window() const { return m_window; }

  private:
    int event_loop();

    // The main application Window
    Window* m_window;

    // Exit flag
    bool m_must_exit;
};

}  // namespace lag

#endif  // LAG_APPLICATION_HPP