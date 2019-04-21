#pragma once

#include "Action.h"
#include "Color.h"
#include "Device.h"

class ColorExtrapolator
{
  private:
    RGB _from;
    RGB _to;
    int _deltaR;
    int _deltaG;
    int _deltaB;
    int _duration;
    unsigned long _start;

  public:
    ColorExtrapolator(uint32_t from, uint32_t to, int duration)
    {
      Configure(from, to, duration);
    }

    void Configure(uint32_t from, uint32_t to, int duration)
    {
      _from.color = from;
      _to.color = to;
      _duration = duration;
      _deltaR = (int)_to.r - (int)_from.r;
      _deltaG = (int)_to.g - (int)_from.g;
      _deltaB = (int)_to.b - (int)_from.b;
    }

    void Setup(Context& context)
    {
      _start = context.now;
    }

    uint32_t Step(Context& context)
    {
      long elapsed = context.now - _start;
      if (elapsed >= _duration)
      {
        return _to.color;
      }

      RGB c;

      c.r = _from.r + (elapsed * _deltaR) / _duration;
      c.g = _from.g + (elapsed * _deltaG) / _duration;
      c.b = _from.b + (elapsed * _deltaB) / _duration;

      return c.color;
    }
};

class DeviceExtrapolateAction : public Action
{
  private:
    Device* _pDevice;     
    ColorExtrapolator* _pExtrapolator;
    RGB _from;
    RGB _to;
    int _duration;
    unsigned long _start;

  public:
    DeviceExtrapolateAction(Device* pDevice, uint32_t from, uint32_t to, int duration) 
    : _pDevice(pDevice), _duration(duration)
    {
      _from.color = from;
      _to.color = to;
    }

    void Setup(Context& context)
    {
      _pExtrapolator = new ColorExtrapolator(_from.color, _to.color, _duration);
      _pExtrapolator->Setup(context);      
      _start = context.now;
    }

    bool Step(Context& context)
    {
      long elapsed = context.now - _start;
      
      auto c = _pExtrapolator->Step(context);
      context.showNeeded |= _pDevice->SetColor(context.strip, c);

      return elapsed < _duration;
    }

    void Teardown(Context& context)
    {
      delete _pExtrapolator;
    }
};

class ExtrapolateAction : public Action
{
  private:
    ColorExtrapolator* _pExtrapolator;
    RGB _from;
    RGB _to;
    int _duration;
    bool _fromSpecified = false;
    unsigned long _start;

  public:
    ExtrapolateAction(uint32_t from, uint32_t to, int duration) : _duration(duration)
    {
      _from.color = from;
      _to.color = to;
      _fromSpecified = true;
    }

    ExtrapolateAction(uint32_t to, int duration) : _duration(duration)
    {
      _to.color = to;
    }

    void Setup(Context& context)
    {
      if (!_fromSpecified)
      {
        _from = context.lastColor;
      }

      _pExtrapolator = new ColorExtrapolator(_from.color, _to.color, _duration);
      _pExtrapolator->Setup(context);      

      _start = context.now;
    }

    bool Step(Context& context)
    {
      long elapsed = context.now - _start;
      
      auto c = _pExtrapolator->Step(context);
      Action::setAll(context, c);
      return elapsed < _duration;
    }

    void Teardown(Context& context)
    {
      delete _pExtrapolator;
    }
};
