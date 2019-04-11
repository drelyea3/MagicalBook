#pragma once

#include "context.h"
#include "watchdog.h"

class PinIO
{
  public:
    typedef void fnValueChangedHandler(int value);
  private:
    const static int MAX_HANDLERS = 4;
    fnValueChangedHandler* _handlers[MAX_HANDLERS];

  protected:
    int _pin;
    int _value = -1;
    Watchdog* _pWatchdog = NULL;

    virtual bool CheckStateCore() = 0;

  public:
    PinIO(int pin) : _pin(pin)
    {
      for (int i = 0; i < MAX_HANDLERS; ++i)
      {
        _handlers[i] = NULL;
      }
    }

    int GetValue() {
      return _value;
    }

    bool CheckState(Context& context)
    {
      auto result = CheckStateCore();
      if (result)
      {
        for (int index = 0; index < MAX_HANDLERS; ++index)
        {
          if (_handlers[index])
          {
            _handlers[index](_value);
          }
        }

        if (_pWatchdog)
        {
          _pWatchdog->Pat(context);
        }
      }

      return result;
    }

    int AddHandler(fnValueChangedHandler* pHandler)
    {
      for (int index = 0; index < MAX_HANDLERS; ++index)
      {
        if (_handlers[index] == NULL)
        {
          _handlers[index] = pHandler;
          return index;
        }
      }

      return -1;
    }

    void RemoveHandler(int token)
    {
      _handlers[token] = NULL;
    }

    void SetWatchdog(Watchdog* pWatchdog)
    {
      _pWatchdog = pWatchdog;
    }
};
