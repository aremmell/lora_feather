#ifndef _LORA_SHARED_H_INCLUDED
#define _LORA_SHARED_H_INCLUDED

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <Fonts/FreeSans12pt7b.h>
#include <aremmel_glyphs.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#define TFT_HDR_COLOR    0x3dff
#define TFT_SIGNAL_COLOR 0xd9bf
#define TFT_UPTIME_COLOR 0x653e
#define TFT_LAST_COLOR   0xf7de
#define TFT_HDR_HEIGHT   20
#define TFT_WIDTH        240
#define TFT_HEIGHT       135
#define TFT_FONT         FreeSans12pt7b
#define BUTTON_D0        0
#define BUTTON_D1        1
#define BUTTON_D2        2

#define SERIAL_BAUD              115200
#define STARTUP_DELAY            100
#define RX_WAIT_TIMEOUT          1000
#define CLIENT_ADDR              0x69
#define SERVER_ADDR              0x70
#define LED_FLASH_INTERVAL       100
#define LED_SETUP_ERROR_INTERVAL 1000

#define RFM95_FREQ  915.0 // MHz
#define RFM95_TXPWR 20    // 5-20 dBm
#define RFM95_RST   11    // A
#define RFM95_CS    10    // B
#define RFM95_IRQ   9     // C

class lora_shared
{
public:
  lora_shared()
  {
    _started_at = millis();
  }

  ~lora_shared() = default;

  static void on_setup_error(Adafruit_NeoPixel& pixel)
  {
    while (true) {
      pixel.setPixelColor(0, 0xff, 0x00, 0x00);
      pixel.show();
      delay(LED_SETUP_ERROR_INTERVAL);
      pixel.setPixelColor(0, 0x00, 0x00, 0x00);
      pixel.show();
      delay(LED_SETUP_ERROR_INTERVAL);
    }
  }

  static void flash_led(uint8_t pin, size_t times)
  {
    for (size_t n = 0; n < times; n++) {
      digitalWrite(pin, HIGH);
      delay(LED_FLASH_INTERVAL);
      digitalWrite(pin, LOW);
      delay(LED_FLASH_INTERVAL);
    }
  }

  static void print_centered(GFXcanvas16& canvas, int16_t x, int16_t y,
    const char* str)
  {
    int16_t x1 = 0;
    int16_t y1 = 0;
    uint16_t cx = 0;
    uint16_t cy = 0;
    canvas.getTextBounds(str, x, y, &x1, &y1, &cx, &cy);
    canvas.setCursor((TFT_WIDTH / 2) - (cx / 2), y);
    canvas.println(str);
  }

  static const char* elapsed_str(u_long since /*msec*/)
  {
    static const size_t len = 14;
    static char buf[len] = {0};

    u_long delta = millis() - since;
    u_long delta_secs = delta / 1000UL;
    uint8_t days = 0;
    uint8_t hours = 0;
    uint8_t mins = 0;
    uint8_t secs = 0;

    if (delta_secs >= 86400) {
      div_t res = div(delta_secs, 86400);
      days = res.quot;
      hours = res.rem / 3600;
      mins = (res.rem % 3600) / 60;
      secs = (res.rem % 3600) % 60;
    } else if (delta_secs >= 3600) {
      div_t res = div(delta_secs, 3600);
      hours = res.quot;
      mins = res.rem / 60;
      secs = res.rem % 60;
    } else if (delta_secs >= 60) {
      mins = delta_secs / 60;
      secs = delta_secs % 60;
    } else {
      secs = delta_secs;
    }

    if (days > 0) {
      snprintf(buf, len, "%hhud%hhuh%hhum%hhus", days, hours, mins, secs);
    } else if (hours > 0) {
      snprintf(buf, len, "%hhuh%hhum%hhus", hours, mins, secs);
    } else if (mins > 0) {
      snprintf(buf, len, "%hhum%hhus", mins, secs);
    } else if (secs > 0) {
      snprintf(buf, len, "%hhus", secs);
    } else {
      snprintf(buf, len, "%s", "<1s");
    }

    return &buf[0];
  }

  const char* uptime_str() const
  {
    return elapsed_str(_started_at);
  }

  static uint8_t bars_from_rssi(int16_t rssi)
  {
    if (rssi == 0) {
      return 0;
    }
    if (rssi >= -55) {
      return 5;
    }
    if (rssi >= -75) {
      return 4;
    }
    if (rssi >= -95) {
      return 3;
    }
    if (rssi >= -115) {
      return 2;
    }
    if (rssi >= -135) {
      return 1;
    }
    return 0;
  }

  static uint16_t color_from_batt_pct(float pct)
  {
    if (pct <= 25.0f) {
      return ST77XX_RED;
    }
    if (pct <= 50.0f) {
      return ST77XX_ORANGE;
    }
    if (pct <= 75.0f) {
      return ST77XX_YELLOW;
    }
    return ST77XX_GREEN;
  }

  static uint32_t neopixel_wheel(uint8_t idx)
  {
    idx = 255 - idx;
    if (idx < 85) {
      return Adafruit_NeoPixel::Color(255 - idx * 3, 0, idx * 3);
    }
    if (idx < 170) {
      idx -= 85;
      return Adafruit_NeoPixel::Color(0, idx * 3, 255 - idx * 3);
    }
    idx -= 170;
    return Adafruit_NeoPixel::Color(idx * 3, 255 - idx * 3, 0);
  }

private:
  u_long _started_at = 0UL;
};

#endif // !_LORA_SHARED_H_INCLUDED
