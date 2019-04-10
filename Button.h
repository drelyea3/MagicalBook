#pragma once

#include "PinIO.h"

class Button : public PinIO
{
  private:
    const int DELAY = 50;
    int _previousValue = 0;
    int _lastRead = 0;
    unsigned long _lastDebounceTime = 0;
  public:
    Button(int pin) : PinIO(pin)
    {
      pinMode(_pin, INPUT_PULLUP);
    }

    bool CheckStateCore()
    {
      auto now = millis();
      bool currentRead = digitalRead(3) == 0;

      if (currentRead != _lastRead)
      {
        _lastDebounceTime = now;
        _lastRead = currentRead;
        return false;
      }

      if (now - _lastDebounceTime > DELAY)
      {
        if (_lastRead != _value)
        {
          _value = _lastRead;
          return true;
        }
      }
      return false;
    }
};
