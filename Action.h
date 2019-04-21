#pragma once

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
  Repeat,     // count
  EndRepeat
};
