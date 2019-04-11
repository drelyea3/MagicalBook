#pragma once

#include "context.h"

class Watchdog
{
  private:
    unsigned long _lastPoke = millis();
    unsigned long _timeout;
    bool _autoReset;
  public:
    Watchdog(unsigned long timeout, bool autoReset = false) : _timeout(timeout), _autoReset(autoReset)
    {
    }

    void Pat(Context& context)
    {
      _lastPoke = context.now;
    }

    bool IsTimeout(Context& context)
    {
      auto result = context.now - _lastPoke > _timeout;
      if (result & _autoReset)
      {
        Pat(context);
      }
      return result;
    }
};
