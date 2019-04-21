#pragma once

#include "Color.h"
#include "context.h"
#include "Button.h"

enum ActionType : uint32_t
{  
  None,       // none (not valid except as initial value)
  Terminate,  // none
  Wait,       // time in ms
  WaitButton, // button pin
  Color1,     // duration, from, to
  Color2,     // duration, from0, to0, from1, to1
  ColorTo1,   // duration, to
  ColorTo2,   // duration, to0, to1
};

class Action
{
  public:
    virtual void Setup(Context& context) {};
    virtual bool Step(Context& context) = 0;
    virtual void Teardown(Context& context) {};

    static void setAll(Context& context, uint32_t color)
    {
      //context.strip.fill(color);
      //context.showNeeded = true;
      context.lastColor.color = color;
    }
};

class TerminateAction : public Action
{
  public:
    void Setup(Context& context)
    {
      Action::setAll(context, BLACK);
    }

    bool Step(Context& context) 
    {
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

    void Setup(Context& context)
    {
      _start = context.now;
    }

    bool Step(Context& context)
    {
      return context.now - _start < _delay;
    }
};

class WaitForButton : public Action
{
  private:
    Button* _pButton;
    bool _isPressed;
  public:
    WaitForButton(Button* pButton, bool pressed) : _pButton(pButton), _isPressed(pressed)
    {
    }

    void Setup(Context& context)
    {
      Serial.println("Waiting for button press");
    }

    bool Step(Context& context)
    {
      return _isPressed == !_pButton->GetValue();
    }

    void Teardown(Context& context)
    {
      Serial.println("Got button press");
    }
};
