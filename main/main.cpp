#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "custom_panel.h"
#include "gfx.hpp"
#include "panel.h"
#include "uix.hpp"

#define TELEGRAMA_IMPLEMENTATION
#include "telegrama.hpp"
#undef TELEGRAMA_IMPLEMENTATION

#define WIFI_SSID "qux"
#define WIFI_PSK "wi9NNYara"

void app_task(void* arg) {
  for (;;) {
    puts("Testing");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

extern "C" void app_main(void) {
  xTaskCreatePinnedToCore(app_task, "app", 4096, nullptr, 5, nullptr, 1);
}
