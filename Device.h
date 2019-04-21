#pragma once

#include <Adafruit_NeoPixel.h>

class Device
{
  private:
    int _pixelStart;
    int _pixelCount;
    uint32_t _color;

  public:
    Device(int start, int count)
      : _pixelStart(start), _pixelCount(count), _color(0)
    {
    }

    Device()
      : _pixelStart(0), _pixelCount(0), _color(0)
    {
    }

    bool SetColor(Adafruit_NeoPixel& strip, uint32_t value)
    {
      if (value != _color)
      {
        _color = value;
        strip.fill(_color, _pixelStart, _pixelCount);
        return true;
      }
      return false;
    }
};
