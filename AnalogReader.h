#pragma once

#include "PinIO.h"

// https://www.megunolink.com/documentation/arduino-libraries/exponential-filter/
#include "Filter.h"

class AnalogReader : public PinIO
{
  private:
    static const int DEFAULT_TOLERANCE = 4;
    int _tolerance;
    ExponentialFilter<long> _inputFilter = ExponentialFilter<long>(20, 0);
    bool _initialValueSet = false;
    unsigned long _lastRead;
    
  protected:
    bool CheckStateCore(unsigned long now)
    {      
      int currentValue = analogRead(_pin);
      
      if (_initialValueSet)
      {
        _inputFilter.Filter(currentValue);
      }
      else
      {
        _initialValueSet = true;
        _inputFilter.SetCurrent(currentValue);
        _lastRead = now;
      }
      
      auto delta = now - _lastRead;
      if (delta < 1000)
      {
        delay(delta);
      }
      
      int filteredValue = _inputFilter.Current();
      if (abs(filteredValue - _value) >= _tolerance)
      {
        //Serial.print("raw "); Serial.print(currentValue); Serial.print(" filtered "); Serial.println(filteredValue);
        _value = filteredValue;
        return true;
      }
      return false;
    }

  public:
    AnalogReader(int pin, int tolerance = AnalogReader::DEFAULT_TOLERANCE) : PinIO(pin), _tolerance(tolerance)
    {
    }
};
