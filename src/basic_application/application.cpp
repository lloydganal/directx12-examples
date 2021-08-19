#include "application.hpp"

namespace lag {

int ApplicationInterface::run() {
    { // Set default values
        Rectangle rect = {0, 0, 800, 600};
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

        poll_system_events();

        if (m_window.should_close()) {
            m_must_exit = true;
        }
    }

    m_window.destroy();

    on_exit();

    return RV_SUCCESS;
}

void ApplicationInterface::poll_system_events() {
    MSG msg;
    msg.message = NULL;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

}  // namespace lag
