#pragma once

class Watchdog
{
  private:
    unsigned long _lastPoke = millis();
    unsigned long _timeout;
  public:
    Watchdog(unsigned long timeout) : _timeout(timeout)
    {
    }

    void Pat()
    {
      _lastPoke = millis();
    }

    bool IsTimeout()
    {
      auto result = millis() - _lastPoke > _timeout;
      if (result)
      {
        Pat();
      }
      return result;
    }
};
