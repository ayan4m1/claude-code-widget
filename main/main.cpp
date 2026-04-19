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

using namespace gfx;
using namespace uix;

static uix::display lcd;

void panel_lcd_flush_complete(void) {
  // let UIX know the DMA transfer completed
  lcd.flush_complete();
}
static void uix_flush(const rect16& bounds, const void* bitmap, void* state) {
  // similar to LVGL
  panel_lcd_flush(bounds.x1, bounds.y1, bounds.x2, bounds.y2, (void*)bitmap);
}

using screen_t = uix::screen<rgb_pixel<LCD_BIT_DEPTH>>;
using scr_color_t = color<screen_t::pixel_type>;
using uix_color_t = color<uix_pixel>;

using label_t = uix::label<screen_t::control_surface_type>;
using icon_t = uix::image_box<screen_t::control_surface_type>;
using vlabel_t = uix::vlabel<screen_t::control_surface_type>;
using rect_t = uix::painter<screen_t::control_surface_type>;

static tt_font small_font(telegrama, 14, font_size_units::px);
static tt_font loading_font(telegrama, 24, font_size_units::px);
static screen_t main_screen;
static screen_t loading_screen;

static label_t connecting_label;

void draw_screen() {
  while (lcd.dirty()) {
    lcd.update();
  }
}

void app_task(void* arg) {
  for (;;) {
    uint32_t started = pdTICKS_TO_MS(xTaskGetTickCount());

    draw_screen();

    uint16_t elapsed = pdTICKS_TO_MS(xTaskGetTickCount()) - started;

    vTaskDelay(pdMS_TO_TICKS(17 - elapsed));
  }
}

extern "C" void app_main(void) {
  panel_lcd_init();

  lcd.buffer_size(LCD_TRANSFER_SIZE);
  lcd.buffer1((uint8_t*)panel_lcd_transfer_buffer());
  lcd.buffer2((uint8_t*)panel_lcd_transfer_buffer2());
  lcd.on_flush_callback(uix_flush);

  small_font.initialize();
  loading_font.initialize();

  loading_screen.dimensions({LCD_WIDTH, LCD_HEIGHT});
  loading_screen.background_color(scr_color_t::white);

  connecting_label.bounds(srect16(0, 0, LCD_WIDTH, LCD_HEIGHT));
  connecting_label.color(uix_color_t::red);
  connecting_label.font(loading_font);
  connecting_label.text("Connecting...");
  connecting_label.text_justify(uix_justify::center);
  loading_screen.register_control(connecting_label);

  lcd.active_screen(loading_screen);
  while (lcd.dirty()) {
    lcd.update();
  }

  xTaskCreatePinnedToCore(app_task, "app", 4096, nullptr, 5, nullptr, 1);
}
