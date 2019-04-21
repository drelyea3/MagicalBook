#pragma once

#include <Adafruit_NeoPixel.h>
#include "Device.h"

// NeoPixel definitions
#define LED_PIN    6
#define LED_COUNT 40

#include "color.h"

class Context
{
  public:
    unsigned long now;
    RGB lastColor;
    bool showNeeded;
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
};
