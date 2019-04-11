#pragma once

#include "Action.h"
#include "Color.h"

class ExtrapolateAction : public Action
{
  private:
    Adafruit_NeoPixel* _pStrip;
    RGB _from;
    RGB _to;
    int _deltaR;
    int _deltaG;
    int _deltaB;
    int _duration;
    unsigned long _start;
    bool _fromSpecified = false;

  public:
    ExtrapolateAction(uint32_t from, uint32_t to, int duration, Adafruit_NeoPixel* pStrip) : _duration(duration), _pStrip(pStrip)
    {
      _from.color = from;
      _to.color = to;
      _fromSpecified = true;
    }

    ExtrapolateAction(uint32_t to, int duration, Adafruit_NeoPixel* pStrip) : _duration(duration), _pStrip(pStrip)
    {
      _to.color = to;
    }

    void Setup(Context& context)
    {
      if (!_fromSpecified)
      {
        _from.color = _pStrip->getPixelColor(0);
        Serial.print("from ");
        Serial.println(_from.color, HEX);
      }
      
      _start = context.now;
      _deltaR = (int)_to.r - (int)_from.r;
      _deltaG = (int)_to.g - (int)_from.g;
      _deltaB = (int)_to.b - (int)_from.b;
    }

    bool Step(Context& context)
    {
      long elapsed = context.now - _start;
      if (elapsed >= _duration)
      {
        setAll(_pStrip, _to.color);
        return false;
      }

      RGB c;

      c.r = _from.r + (elapsed * _deltaR) / _duration;
      c.g = _from.g + (elapsed * _deltaG) / _duration;
      c.b = _from.b + (elapsed * _deltaB) / _duration;
      setAll(_pStrip, c.color);

      return true;
    }
};
