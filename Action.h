#pragma once

class Action
{
  public:
    virtual void Setup() {};
    virtual bool Step() = 0;
    virtual void Teardown() {};
};
