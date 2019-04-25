#pragma once

union RGB {
  uint32_t color;
  struct {
    byte b;
    byte g;
    byte r;
  };
};

#define BLACK   (uint32_t) 0
#define RED     (uint32_t) 0xFF0000
#define GREEN   (uint32_t) 0x00FF00
#define BLUE    (uint32_t) 0x0000FF
#define WHITE   (uint32_t) 0xFFFFFF
#define YELLOW  (uint32_t) 0xFFFF00
#define COOL    (uint32_t) 0x3399ff
#define GOLD    (uint32_t) 0xFFD700
