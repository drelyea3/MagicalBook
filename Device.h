#pragma once

#include <Adafruit_NeoPixel.h>
#include "ColorExtrapolator.h"
#include "Context.h"

class Device
{
  private:
    uint32_t _pixelStart;
    uint32_t _pixelCount;
    uint32_t _duration;
    uint32_t _color;
    
    ColorExtrapolator _extrapolator;

  public:
    Device(uint32_t start, uint32_t count)
      : _pixelStart(start), _pixelCount(count)
    {
    }

    Device()
      : _pixelStart(0), _pixelCount(0)
    {
    }

    void Extrapolate(Context& context, uint32_t from, uint32_t to, uint32_t duration)
    {
      _extrapolator.Configure(from, to, duration, context.now);
      _duration = duration;
    }

    void Extrapolate(Context& context, uint32_t to, uint32_t duration)
    {
      _extrapolator.Configure(_extrapolator.GetValue(), to, duration, context.now);
      _duration = duration;
    }

    bool Step(Context& context)
    {
      auto more = _extrapolator.Step(context.now);
      SetColor(context, _extrapolator.GetValue());
      return more;
    }

    void SetColor(Context& context, uint32_t value)
    {
      if (value != _color)
      {
        _color = value;
        context.Fill(_color, _pixelStart, _pixelCount);
      }
    }
};
