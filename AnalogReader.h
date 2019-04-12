#pragma once

#include "PinIO.h"

class AnalogReader : public PinIO
{
  private:
    static const int DEFAULT_TOLERANCE = 4;
    int _tolerance;

  protected:
    bool CheckStateCore()
    {
      auto currentValue = analogRead(_pin);
      //Serial.println(currentValue);
      if (abs(currentValue - _value) >= _tolerance)
      {
        _value = currentValue;
        return true;
      }
      return false;
    }

  public:
    AnalogReader(int pin, int tolerance = AnalogReader::DEFAULT_TOLERANCE) : PinIO(pin), _tolerance(tolerance)
    {
    }
};
