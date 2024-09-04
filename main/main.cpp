#include "App.hpp"

pp::App g_app;

extern "C" void app_main(void)
{
    g_app.start();
}
