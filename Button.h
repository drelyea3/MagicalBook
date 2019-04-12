#pragma once

#include "PinIO.h"

class Button : public PinIO
{
  private:
    const int DELAY = 50;
    int _lastRead = -1;
    unsigned long _lastDebounceTime = 0;
    bool _isPressed;
    bool _isReleased;
    
  public:
    Button(int pin) : PinIO(pin)
    {
      pinMode(_pin, INPUT_PULLUP);
    }

    bool IsPressed() { return _isPressed; }
    bool IsReleased() { return _isReleased; }
    bool GetUndebouncedValue() { return _lastRead; }
    
    bool CheckStateCore(unsigned long now)
    {
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
          _isReleased = _value == 1 && _lastRead == 0;
          _isPressed  = _value == 0 && _lastRead == 1;
          _value = _lastRead;
          return true;
        }
      }
      return false;
    }
};
