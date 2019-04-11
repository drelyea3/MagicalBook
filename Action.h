#pragma once

#include "Color.h"
#include "context.h"

class Action
{
  public:
    virtual void Setup(Context& context) {};
    virtual bool Step(Context& context) = 0;
    virtual void Teardown(Context& context) {};
};

#include "Button.h"

class TerminateAction : public Action
{
  private:
    Adafruit_NeoPixel* _pStrip;
  public:
    TerminateAction(Adafruit_NeoPixel* pStrip) : _pStrip(pStrip) {}
    void Setup(Context& context)
    {
      setAll(_pStrip, BLACK);
    }
    
    bool Step(Context& context) {
      return true;
    }
};

class WaitAction : public Action
{
  private:
    unsigned long _start;
    int _delay;

  public:
    WaitAction(int ms) : _delay(ms) {}

    void Setup(Context& context) {
      _start = context.now;
    }
    bool Step(Context& context) {
      return context.now - _start < _delay;
    }
};

class WaitForButton : public Action
{
  private:
    Button* _pButton;
  public:
    WaitForButton(Button* pButton) : _pButton(pButton)
    {
    }

    void Setup(Context& context) {
      Serial.println("Waiting for button press");
    }

    bool Step(Context& context)
    {
      return !_pButton->GetValue();
    }

    void Teardown(Context& context) {
      Serial.println("Got button press");
    }
};
