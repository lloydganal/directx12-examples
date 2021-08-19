

#include "application.hpp"

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
