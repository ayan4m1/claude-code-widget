#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "custom_panel.h"
#include "gfx.hpp"
extern "C" {
#include "nimble-nordic-uart.h"
}
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

static label_t advertising_label;
static rect_t logo_rect;

void draw_screen() {
  while (lcd.dirty()) {
    lcd.update();
  }
}

void lcd_task(void* arg) {
  for (;;) {
    uint32_t started = pdTICKS_TO_MS(xTaskGetTickCount());

    draw_screen();

    uint16_t elapsed = pdTICKS_TO_MS(xTaskGetTickCount()) - started;

    vTaskDelay(pdMS_TO_TICKS(17 - elapsed));
  }
}

void ble_task(void* arg) {
  static char buffer[CONFIG_NORDIC_UART_MAX_LINE_LENGTH + 1];

  for (;;) {
    size_t item_size;
    if (nordic_uart_rx_buf_handle) {
      const char* item = (char*)xRingbufferReceive(nordic_uart_rx_buf_handle,
                                                   &item_size, portMAX_DELAY);
      if (item) {
        int i;
        for (i = 0; i < item_size; ++i) {
          if (item[i] >= 'a' && item[i] <= 'z')
            buffer[i] = item[i] - 0x20;
          else
            buffer[i] = item[i];
        }
        buffer[item_size] = '\0';
        // memcpy(buffer, item, item_size);

        puts(buffer);
        vRingbufferReturnItem(nordic_uart_rx_buf_handle, (void*)item);
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

  vTaskDelete(NULL);
}

void draw_rect() {}

extern "C" void app_main(void) {
  panel_lcd_init();

  lcd.buffer_size(LCD_TRANSFER_SIZE);
  lcd.buffer1((uint8_t*)panel_lcd_transfer_buffer());
  lcd.buffer2((uint8_t*)panel_lcd_transfer_buffer2());
  lcd.on_flush_callback(uix_flush);

  small_font.initialize();
  loading_font.initialize();

  loading_screen.dimensions({LCD_WIDTH, LCD_HEIGHT});
  loading_screen.background_color(scr_color_t::black);

  advertising_label.bounds(srect16(0, 0, LCD_WIDTH, LCD_HEIGHT));
  advertising_label.color(uix_color_t::red);
  advertising_label.font(loading_font);
  advertising_label.text("Advertising...");
  advertising_label.text_justify(uix_justify::center);
  loading_screen.register_control(advertising_label);

  main_screen.dimensions({LCD_WIDTH, LCD_HEIGHT});
  main_screen.background_color(scr_color_t::black);

  logo_rect.bounds(srect16(0, 0, LCD_WIDTH, LCD_HEIGHT));
  main_screen.register_control(logo_rect);

  lcd.active_screen(loading_screen);
  while (lcd.dirty()) {
    lcd.update();
  }

  nordic_uart_start("Claude Code Widget", nullptr);

  xTaskCreatePinnedToCore(lcd_task, "LCD", 4096, nullptr, 5, nullptr, 1);
  xTaskCreatePinnedToCore(ble_task, "BLE", 4096, nullptr, 5, nullptr, 0);
}
