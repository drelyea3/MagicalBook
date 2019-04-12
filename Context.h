#pragma once

#include <Adafruit_NeoPixel.h>

#include "color.h"

class Context
{
  public:
    unsigned long now;
    RGB lastColor;
    bool showNeeded;
};
