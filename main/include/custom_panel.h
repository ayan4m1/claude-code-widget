#ifndef CUSTOM_PANEL_H
#define CUSTOM_PANEL_H

#define LCD_SPI_HOST SPI_3
#define LCD_PIN_NUM_MOSI 1
#define LCD_PIN_NUM_CLK 2
#define LCD_PIN_NUM_CS 3
#define LCD_PIN_NUM_DC 4
#define LCD_PIN_NUM_RST 5
#define LCD_PIN_NUM_BCKL 6
#define LCD_INIT esp_lcd_new_panel_st7789
#define LCD_HRES 240
#define LCD_VRES 280
#define LCD_COLOR_SPACE LCD_COLOR_BGR
#define LCD_CLOCK_HZ (40 * 1000 * 1000)
#define LCD_GAP_X 20
#define LCD_MIRROR_X 1
#define LCD_MIRROR_Y 0
#define LCD_INVERT_COLOR 1
#define LCD_SWAP_COLOR_BYTES 0
#define LCD_SWAP_XY 1
#define LCD_BCKL_PWM_CHANNEL 0
#define BUTTON_MASK BUTTON_PIN(0)
#define BUTTON_ON_LEVEL 0

#endif