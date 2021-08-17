#include "application.hpp"

#include "application_windows.hpp"

namespace lag {


int ApplicationInterface::run(ApplicationArgs& args) { 
    
    return 0; }

int ApplicationInterface::event_loop() {
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

}  // namespace lag
