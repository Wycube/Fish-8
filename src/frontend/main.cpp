#include "Application.hpp"
#include "Log.hpp"

int main(int argc, char *argv[]) {
    Application app;
    if(!app.init(960, 501, "Fish-8", argc, argv)) {
        LOG_FATAL("[APP]: Failed to Initialize!");
    }
    app.loop();
    app.cleanup();
}