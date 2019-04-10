#pragma once

#include <Adafruit_NeoPixel.h>
#include "button.h"

#define LED_PIN    6
#define LED_COUNT 40

class Context
{
  public:
    unsigned long now;
    Adafruit_NeoPixel* pPixels;
};
