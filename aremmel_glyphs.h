#ifndef _AREMMELL_GLYPHS_H_INCLUDED
#define _AREMMELL_GLYPHS_H_INCLUDED

#include <Arduino.h>

static uint8_t _batt_glyph[] = {
  0b01111111, 0b11111111, 0b11111000,
  0b10000000, 0b00000000, 0b00000100,
  0b10000000, 0b00000000, 0b00000100,
  0b10000000, 0b00000000, 0b00000011,
  0b10000000, 0b00000000, 0b00000011,
  0b10000000, 0b00000000, 0b00000011,
  0b10000000, 0b00000000, 0b00000100,
  0b10000000, 0b00000000, 0b00000100,
  0b01111111, 0b11111111, 0b11111000
};

static uint8_t _signal_bars_glyph[] = {
  0b00000000, 0b00000000, 0b00001111,
  0b00000000, 0b00000000, 0b00001001,
  0b00000000, 0b00000001, 0b11101001,
  0b00000000, 0b00000001, 0b00101001,
  0b00000000, 0b00111101, 0b00101001,
  0b00000000, 0b00100101, 0b00101001,
  0b00000111, 0b10100101, 0b00101001,
  0b00000100, 0b10100101, 0b00101001,
  0b11110100, 0b10100101, 0b00101001,
  0b10010100, 0b10100101, 0b00101001,
  0b10010100, 0b10100101, 0b00101001,
  0b11110111, 0b10111101, 0b11101111
};

static uint8_t _up_arrow_glyph[] = {
  0b00000000, 0b00011000, 0b00000000,
  0b00000000, 0b00100100, 0b00000000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b10000001, 0b00000000,
  0b00000001, 0b00000000, 0b10000000,
  0b00000010, 0b00000000, 0b01000000,
  0b00000100, 0b00000000, 0b00100000,
  0b00001000, 0b00000000, 0b00010000,
  0b00011111, 0b10000001, 0b11111000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b01000010, 0b00000000,
  0b00000000, 0b01111110, 0b00000000,
};

static uint8_t _round_trip_glyph[] = {
  0b00000100, 0b00000000, 0b00000000,
  0b00001100, 0b00000000, 0b00000000,
  0b00010100, 0b00000000, 0b00000000,
  0b00100100, 0b00000000, 0b00000000,
  0b01000111, 0b11111110, 0b00000000,
  0b10000000, 0b00000010, 0b00100000,
  0b10000000, 0b00000010, 0b00110000,
  0b01000111, 0b11111110, 0b00101000,
  0b00100100, 0b00000000, 0b00100100,
  0b00010100, 0b01111111, 0b11100010,
  0b00001100, 0b01000000, 0b00000001,
  0b00000100, 0b01000000, 0b00000001,
  0b00000000, 0b01111111, 0b11100010,
  0b00000000, 0b00000000, 0b00100100,
  0b00000000, 0b00000000, 0b00101000,
  0b00000000, 0b00000000, 0b00110000,
  0b00000000, 0b00000000, 0b00100000,
};

// [7,6,5,4,3,2,1,0], [15,14,13,12,11,10,9,8], [23,22,21,20,19,18,17,16]

class aremmell_glyphs
{
public:
  aremmell_glyphs() { };
  ~aremmell_glyphs() { };

  void resetGlyphs() const
  {
    getBatteryGlyph(0, NULL, NULL);
    getSignalGlyph(0, NULL, NULL);
    getUpArrowGlyph(false, NULL, NULL);
  }

  const uint8_t* getBatteryGlyph(uint8_t pct, int16_t* w, int16_t* h) const
  {
    if (w) *w = 24;
    if (h) *h = 9;
    if (pct > 100) pct = 100;

    static const uint8_t offsets[] = {
      6, 9, 12, 15, 18
    };
    static const uint8_t bits[] = {
      5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8, 23, 22, 21, 20
    };
    uint8_t fill = static_cast<uint8_t>(
      floor((18.0f * (pct > 0 ? (pct / 100.0f) : 0.0f)))
    );

    for (uint8_t off = 0; off < sizeof(offsets); off++) {
      uint32_t* p32 = reinterpret_cast<uint32_t*>(&_batt_glyph[offsets[off]]);
      for (uint8_t b = 0; b < sizeof(bits); b++) {
        if (b <= fill)
          *p32 |= 1 << bits[b];
        else
          *p32 &= ~(1 << bits[b]);
      }
    }

    return &_batt_glyph[0];
  }

  const uint8_t* getSignalGlyph(uint8_t bars, int16_t* w, int16_t* h) const
  {
    if (w) *w = 24;
    if (h) *h = 12;
    if (bars > 5) bars = 5;

    static const uint8_t offsets[] = {
      3, 6, 9, 12, 15, 18, 21, 24, 27, 30
    };

    for (uint8_t off = 0; off < sizeof(offsets); off++) {
      uint32_t* p32 = reinterpret_cast<uint32_t*>(&_signal_bars_glyph[offsets[off]]);
      if (bars == 5)
        *p32 |= 0x00060000; // 17, 18
      else
        *p32 &= 0xfff9ffff;
      if (off >= 2) {
        if (bars >= 4)
          *p32 |= 0x00c00000; // 22, 23
        else
          *p32 &= 0xff3fffff;
      }
      if (off >= 4) {
        if (bars >= 3)
          *p32 |= 0x00001800; // 11, 12
        else
          *p32 &= 0xffffe7ff;
      }
      if (off >= 6) {
        if (bars >= 2)
          *p32 |= 0x00000003; // 0, 1
        else
          *p32 &= 0xfffffffc;
      }
      if (off >= 8) {
        if (bars >= 3)
          *p32 |= 0x00000060; // 5, 6
        else
          *p32 &= 0xffffffff;
      }
    }

    return &_signal_bars_glyph[0];
  }

  const uint8_t* getUpArrowGlyph(bool filled, int16_t* w, int16_t* h) const
  {
    if (w) *w = 24;
    if (h) *h = 16;

    static const uint8_t offsets[] = {
      3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42
    };

    for (uint8_t off = 0; off < sizeof(offsets); off++) {
      if (off < 4 || off > 6) {
        uint8_t byte2;
        switch (offsets[off]) {
          case 3:  byte2 = filled ? 0x3c : 0x24; break;
          case 6:  byte2 = filled ? 0x7e : 0x42; break;
          case 9:  byte2 = filled ? 0xff : 0x81; break;
          case 12: byte2 = filled ? 0xff : 0x00; break;
          case 24: byte2 = filled ? 0xff : 0x81; break;
          default: byte2 = filled ? 0x7e : 0x42; break;
        }
        _up_arrow_glyph[offsets[off] + 1] = byte2;
      } else {
        uint8_t byte1, byte2, byte3;
        switch (offsets[off]) {
          case 15:
            byte1 = filled ? 0x03 : 0x02;
            byte2 = filled ? 0xff : 0x00;
            byte3 = filled ? 0xc0 : 0x40;
          break;
          case 18:
            byte1 = filled ? 0x07 : 0x04;
            byte2 = filled ? 0xff : 0x00;
            byte3 = filled ? 0xe0 : 0x20;
          break;
          case 21:
            byte1 = filled ? 0x0f : 0x08;
            byte2 = filled ? 0xff : 0x00;
            byte3 = filled ? 0xf0 : 0x10;
          break;
        }
        _up_arrow_glyph[offsets[off]] = byte1;
        _up_arrow_glyph[offsets[off] + 1] = byte2;
        _up_arrow_glyph[offsets[off] + 2] = byte3;
      }
    }

    return &_up_arrow_glyph[0];
  }

  const uint8_t* getRTTGlyph(bool filled, int16_t* w, int16_t* h) const
  {
    if (w) *w = 24;
    if (h) *h = 17;

    static const uint8_t offsets[] = {
      6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42
    };

    for (uint8_t off = 0; off < sizeof(offsets); off++) {
      uint32_t* p32 = reinterpret_cast<uint32_t*>(&_round_trip_glyph[offsets[off]]);
      switch (offsets[off]) {
        case 6:
          if (filled)
            *p32 |= 0x00000008; // 3
          else
            *p32 &= 0xfffffff7;
        break;
        case 9:
          if (filled)
            *p32 |= 0x00000018; // 4, 3
          else
            *p32 &= 0xffffffe7;
        break;
        case 12:
          if (filled)
            *p32 |= 0x00000038; // 5, 4, 3
          else
            *p32 &= 0xffffffc7;
        break;
        case 15:
        case 18:
          if (filled)
            *p32 |= 0x0000fc7f; // 6..0, 15..10
          else
            *p32 &= 0xffff0380;
        break;
        case 21:
          if (filled)
            *p32 |= 0x00100038; // 5, 4, 3, 20
          else
            *p32 &= 0xffefffc7;
        break;
      /*
  0b00000100, 0b00000000, 0b00000000, // --
  0b00001100, 0b00000000, 0b00000000, // --
  0b00010100, 0b00000000, 0b00000000, // 6
  0b00100100, 0b00000000, 0b00000000, // 9
  0b01000111, 0b11111110, 0b00000000, // 12
  0b10000000, 0b00000010, 0b00100000, //15
  0b10000000, 0b00000010, 0b00110000, //18
  0b01000111, 0b11111110, 0b00101000, //21
  0b00100100, 0b00000000, 0b00100100, //24
  0b00010100, 0b01111111, 0b11100010, //27
  0b00001100, 0b01000000, 0b00000001, //30
  0b00000100, 0b01000000, 0b00000001, //33
  0b00000000, 0b01111111, 0b11100010, //36
  0b00000000, 0b00000000, 0b00100100, //39
  0b00000000, 0b00000000, 0b00101000, //42
  0b00000000, 0b00000000, 0b00110000, //--
  0b00000000, 0b00000000, 0b00100000, //--
*/
// [7,6,5,4,3,2,1,0], [15,14,13,12,11,10,9,8], [23,22,21,20,19,18,17,16]
        case 24:
          if (filled)
            *p32 |= 0x00180018; // 4, 3, 20, 19
          else
            *p32 &= 0xffe7ffe7;
        break;
        case 27:
          if (filled)
            *p32 |= 0x001c0008; // 3, 20, 19, 18
          else
            *p32 &= 0xffe3fff7;
        break;
        case 30:
        case 33:
          if (filled)
            *p32 |= 0x00fe3f00; // 13..8, 23..17
          else
            *p32 &= 0xfff1c0ff;
        break;
        case 36:
          if (filled)
            *p32 |= 0x001c0000; // 20, 19, 18
          else
            *p32 &= 0xffe3ffff;
        break;
        case 39:
          if (filled)
            *p32 |= 0x00180000; // 20, 19
          else
            *p32 &= 0xffe7ffff;
        break;
        case 42:
          if (filled)
            *p32 |= 0x00100000; // 20
          else
            *p32 &= 0xffefffff;
        break;
      }
    }

      /*uint8_t byte1, byte2, byte3;
      switch (offsets[off]) {
        case 6:
        byte1 = filled ? 0x1c : 0x14;
        byte2 = 0x00;
        byte3 = 0x00;
        break;
        case 9:
        byte1 = filled ? 0x3c :0x24;
        byte2 = 0x00;
        byte3 = 0x00;
        break;
        case 12:
        byte1 = filled ? 0x7f : 0x47;
        byte2 = 0xfe;
        byte3 = 0x00;
        break;
        case 15:
        byte1 = filled ? 0xff : 0x80;
        byte2 = filled ? 0xfe : 0x02;
        byte3 = 0x20;
        break;
        case 18:
        byte1 = filled ? 0xff : 0x80;
        byte2 = filled ? 0xfe : 0x02;
        byte3 = 0x30;
        break;
        case 21:
        byte1 = filled ? 0x7f : 0x47;
        byte2 = 0xfe;
        byte3 = filled ? 0x38 : 0x28;
        break;
        case 24:
        byte1 = filled ? 0x3c : 0x24;
        byte2 = 0x00;
        byte3 = filled ? 0x3c : 0x24;
        break;
        case 27:
        byte1 = filled ? 0x1c : 0x14;
        byte2 = 0x7f;
        byte3 = filled ? 0xfe : 0xe2;
        break;
        case 30:
        byte1 = 0x0c;
        byte2 = filled ? 0x7f : 0x40;
        byte3 = filled ? 0xff : 0x01;
        break;
        case 33:
        byte1 = 0x04;
        byte2 = filled ? 0x7f : 0x40;
        byte3 = filled ? 0xff : 0x01;
        break;
        case 36:
        byte1 = 0x00;
        byte2 = 0x7f;
        byte3 = filled ? 0xfe : 0xe2;
        break;
        case 39:
        byte1 = 0x00;
        byte2 = 0x00;
        byte3 = filled ? 0x3c : 0x24;
        break;
        case 42:
        byte1 = 0x00;
        byte2 = 0x00;
        byte3 = filled ? 0x38 : 0x28;
        break;
      }
      _round_trip_glyph[offsets[off]] = byte1;
      _round_trip_glyph[offsets[off] + 1] = byte2;
      _round_trip_glyph[offsets[off] + 2] = byte3;
    }*/
    return &_round_trip_glyph[0];
  }
};

#endif // !_AREMMELL_GLYPHS_H_INCLUDED
