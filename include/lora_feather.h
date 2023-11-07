#ifndef _LORA_FEATHER_H_INCLUDED
#define _LORA_FEATHER_H_INCLUDED

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

class lora_feather
{
public:
  explicit lora_feather(uint8_t address)
    : _pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800), _rf95(RFM95_CS, RFM95_IRQ),
      _rfmgr(_rf95, address), _display(TFT_CS, TFT_DC, TFT_RST),
      _canvas(TFT_WIDTH, TFT_HEIGHT)
  {
    _started_at = millis();
  }

  lora_feather() = delete;
  ~lora_feather() = default;

  /// @brief call from setup().
  void begin()
  {
    u_long wait = millis();
    while (!Serial) {
      delay(10);
      if (millis() - wait > STARTUP_DELAY)
        break;
    }

    Serial.begin(SERIAL_BAUD);
    Serial.print("LoRa Feather @"); Serial.print(RFM95_FREQ, 1); Serial.println("MHz");

    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    _pixel.begin();
    _pixel.setBrightness(20);
    _pixel.fill(0xffffff);
    _pixel.show();

    if (!_rfmgr.init()) {
      Serial.println("Radio init failed!");
      on_setup_error();
    }
    Serial.println("Radio init OK!");

    if (!_rf95.setFrequency(RFM95_FREQ)) {
      Serial.println("Failed to set freq!");
      on_setup_error();
    }
    Serial.print("Freq set to: "); Serial.print(RFM95_FREQ); Serial.println("MHz");

    _rf95.setTxPower(RFM95_TXPWR);
    Serial.print("Tx power set to: "); Serial.print(RFM95_TXPWR); Serial.println("dBm");

    _display.init(TFT_HEIGHT, TFT_WIDTH);
    _display.setRotation(3);

    _canvas.setFont(&TFT_FONT);
    _canvas.setTextColor(ST77XX_WHITE);

    if (!_lipo.begin()) {
      Serial.println("MAX17048 not found!");
      on_setup_error();
    }

    Serial.print("MAX17048 OK; ID: 0x"); Serial.println(_lipo.getChipID(), HEX);
  }

  void on_setup_error()
  {
    while (true) {
      _pixel.setPixelColor(0, 0xff, 0x00, 0x00);
      _pixel.show();
      delay(LED_SETUP_ERROR_INTERVAL);
      _pixel.setPixelColor(0, 0x00, 0x00, 0x00);
      _pixel.show();
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

  RH_RF95& get_rf95() { return _rf95; }
  RHReliableDatagram& get_reliable_dg() { return _rfmgr; }
  Adafruit_NeoPixel& get_pixel() { return _pixel; }
  Adafruit_MAX17048& get_lipo() { return _lipo; }
  Adafruit_ST7789& get_display() { return _display; }
  GFXcanvas16& get_canvas() { return _canvas; }

private:
  u_long _started_at = 0UL;
  RH_RF95 _rf95;
  RHReliableDatagram _rfmgr;
  Adafruit_NeoPixel _pixel;
  Adafruit_MAX17048 _lipo;
  Adafruit_ST7789 _display;
  GFXcanvas16 _canvas;
};

#endif // !_LORA_FEATHER_H_INCLUDED
