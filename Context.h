#pragma once

#include <Adafruit_NeoPixel.h>

// NeoPixel definitions
#define LED_PIN    6
#define LED_COUNT 40

#include "color.h"

class Context
{
  public:
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
    bool showNeeded;

    unsigned long now;
    RGB lastColor;

    void Fill(uint32_t color, uint32_t first = 0, uint32_t count = 0)
    {
      strip.fill(color, (uint16_t) first, (uint16_t) count);
      showNeeded = true;
    }
};
