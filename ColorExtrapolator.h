#pragma once

#include "Color.h"

class ColorExtrapolator
{
  private:
    RGB _from;
    RGB _to;
    int _deltaR;
    int _deltaG;
    int _deltaB;
    int _duration;
    RGB _value;
    unsigned long _start;

  public:
    ColorExtrapolator()
    {
      _value.color = 0;
    }

    uint32_t GetValue() 
    {
      return _value.color;
    }

    void Configure(uint32_t from, uint32_t to, int duration, unsigned long start)
    {
      _from.color = from;
      _to.color = to;
      _duration = duration;
      _deltaR = (int)_to.r - (int)_from.r;
      _deltaG = (int)_to.g - (int)_from.g;
      _deltaB = (int)_to.b - (int)_from.b;
      _start = start;
    }

    uint32_t Step(unsigned long now)
    {
      long elapsed = now - _start;
      if (elapsed >= _duration)
      {
        _value = _to;
        return false;
      }

      RGB c;

      c.r = _from.r + (elapsed * _deltaR) / _duration;
      c.g = _from.g + (elapsed * _deltaG) / _duration;
      c.b = _from.b + (elapsed * _deltaB) / _duration;

      _value = c;

      return true;
    }
};
