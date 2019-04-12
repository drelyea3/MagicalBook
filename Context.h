#pragma once

#include <Adafruit_NeoPixel.h>

#include "color.h"

#define LED_PIN    6
#define LED_COUNT 40

class Context
{
  public:
    unsigned long now;
    RGB lastColor;
};
