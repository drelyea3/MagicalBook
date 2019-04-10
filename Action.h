#pragma once

#include "Color.h"

class Action
{
  public:
    virtual void Setup() {};
    virtual bool Step() = 0;
    virtual void Teardown() {};
};

#include "Button.h"

class TerminateAction : public Action
{
  private:
    Adafruit_NeoPixel* _pStrip;
  public:
    TerminateAction(Adafruit_NeoPixel* pStrip) : _pStrip(pStrip) {}
    void Setup() 
    {
      setAll(_pStrip, BLACK);
    }
    
    bool Step() {
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

    void Setup() {
      _start = millis();
    }
    bool Step() {
      return millis() - _start < _delay;
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

    void Setup() {
      Serial.println("Waiting for button press");
    }

    bool Step()
    {
      return !_pButton->GetValue();
    }

    void Teardown() {
      Serial.println("Got button press");
    }
};
