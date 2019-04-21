#define NO_INPUT 1

#include "Context.h"
#include "Color.h"
#include "Device.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"

#define TRACE 1

#define BRIGHTNESS 255

#define ACTIVATE_BUTTON_PIN 4
#define POWER_LED_PIN 5
#define POWER_LED_BRIGHTNESS 8

#define BRIGHTNESS_PIN 3
#define BRIGHTNESS_TOLERANCE 8

Button pushButton(ACTIVATE_BUTTON_PIN);

// The one and only global application context
Context g_context;

#define DEVICE_LED_COUNT (LED_COUNT / 2)
Device dev0(0, DEVICE_LED_COUNT);
Device dev1(DEVICE_LED_COUNT, DEVICE_LED_COUNT);

#define WAIT(D) ActionType::Wait, D
#define COLOR_1(D, FROM, TO) ActionType::Color1, D, FROM, TO
#define COLOR_2(D, FROM1, TO1, FROM2, TO2) ActionType::Color2, D, FROM1, TO1, FROM1, TO2
#define COLOR_TO_1(D, TO) ActionType::ColorTo1, D, TO
#define COLOR_TO_2(D, TO1, TO2) ActionType::ColorTo2, D, TO1, TO2

#define TIME 500

const static uint32_t actionData[] = {
  ActionType::Repeat, 4,
  COLOR_2(TIME, BLACK, BLUE, BLACK, GREEN),
  COLOR_2(TIME, BLACK, GREEN, BLACK, BLUE),
  ActionType::EndRepeat,

  COLOR_1(1000, BLACK, BLACK),
  ActionType::Repeat, 4,
  COLOR_TO_2(TIME, RED, WHITE),
  COLOR_TO_2(TIME, WHITE, BLUE),
  COLOR_TO_2(TIME, BLUE, WHITE),
  COLOR_TO_2(TIME, WHITE, GREEN),
  COLOR_TO_2(TIME, GREEN, WHITE),
  ActionType::EndRepeat,

  COLOR_1(1000, BLACK, BLACK),
  ActionType::Repeat, 5,
  COLOR_2(20, WHITE, BLACK, WHITE, BLACK),
  ActionType::EndRepeat,

  COLOR_1(1000, BLACK, BLACK),
  ActionType::Repeat, 5,
  COLOR_2(50, WHITE, BLACK, WHITE, BLACK),
  ActionType::EndRepeat,

  COLOR_1(1000, BLACK, BLACK),
  ActionType::Repeat, 5,
  COLOR_2(100, WHITE, BLACK, WHITE, BLACK),
  ActionType::EndRepeat,

  COLOR_1(1000, BLACK, BLACK),
  COLOR_TO_2(500, WHITE, RED),
  COLOR_TO_2(500, GREEN, BLUE),
  COLOR_TO_2(500, RED, GREEN),
  COLOR_TO_2(500, BLUE, RED),
  COLOR_TO_2(500, YELLOW, YELLOW),
  COLOR_TO_2(500, WHITE, WHITE),
  ActionType::Terminate
};

ActionType currentAction = ActionType::None;
int __nextActionData__ = 0;

uint32_t GetNext()
{
  auto value = actionData[__nextActionData__++];
  //Serial.print("GetNext "); Serial.println(value, HEX);
  return value;
}

ActionType GetNextAction()
{
  return (ActionType) GetNext();
}

bool actionFinished = true;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pushButton.CheckState(g_context);
  g_context.strip.begin();
  g_context.strip.setBrightness(BRIGHTNESS);
  dev0.SetColor(g_context, 0);
  dev1.SetColor(g_context, 0);
  g_context.strip.show();
}

unsigned long _waitStart;
uint32_t _waitDuration;

#define MAX_REPEAT 4
uint16_t repeatIndex = 0;
uint16_t repeatCount[MAX_REPEAT];
uint16_t repeatAction[MAX_REPEAT];

void SetupAction(Context& context, ActionType action)
{
  switch (action)
  {
    case ActionType::Wait:
      {
        _waitStart = context.now;
        _waitDuration = GetNext();
      }
      break;

    case ActionType::Color1:
      {
        auto duration = GetNext();
        auto from = GetNext();
        auto to = GetNext();
        dev0.Extrapolate(context, from, to, duration);
        dev1.Extrapolate(context, from, to, duration);
      }
      break;
    case ActionType::Color2:
      {
        auto duration = GetNext();
        auto from = GetNext();
        auto to = GetNext();
        dev0.Extrapolate(context, from, to, duration);
        from = GetNext();
        to = GetNext();
        dev1.Extrapolate(context, from, to, duration);
      }
      break;
    case ActionType::ColorTo1:
      {
        auto duration = GetNext();
        auto to = GetNext();
        dev0.Extrapolate(context, to, duration);
        dev1.Extrapolate(context, to, duration);
      }
      break;
    case ActionType::ColorTo2:
      {
        auto duration = GetNext();
        auto to = GetNext();
        dev0.Extrapolate(context, to, duration);
        to = GetNext();
        dev1.Extrapolate(context, to, duration);
      }
      break;
    case ActionType::Repeat:
      {
        auto count = GetNext();
        if (repeatIndex < MAX_REPEAT)
        {
          repeatAction[repeatIndex] = __nextActionData__;
          repeatCount[repeatIndex] = count;
          ++repeatIndex;
          actionFinished = true;
        }
      }
      break;
    case ActionType::EndRepeat:
      {
        int index = repeatIndex - 1;
        if (index >= 0)
        {
          if (--repeatCount[index])
          {
            __nextActionData__ = repeatAction[index];
          }
          else
          {
            --repeatIndex;
          }
          actionFinished = true;
        }
      }
      break;
    case ActionType::Terminate:
      dev0.SetColor(context, 0);
      dev1.SetColor(context, 0);
      break;
    default:
      Serial.print("Unknown action in SetupAction "); Serial.println(action);
      delay(10000);
  }
}

void Step(Context& context, ActionType action)
{
  switch (action)
  {
    case ActionType::Color1:
    case ActionType::ColorTo1:
    case ActionType::Color2:
    case ActionType::ColorTo2:
      {
        auto more = dev0.Step(context);
        dev1.Step(context);
        if (!more)
        {
          actionFinished = true;
        }
      }
      break;
    case ActionType::Wait:
      actionFinished = context.now - _waitStart >= _waitDuration;
      break;
    default:
      Serial.print("Unknown action in step "); Serial.println(action);
      delay(10000);
  }
}

void loop()
{
  g_context.now = millis();

  auto pushButtonChanged = pushButton.CheckState(g_context);

  if (actionFinished && currentAction != ActionType::Terminate)
  {
    actionFinished = false;
    currentAction = GetNextAction();
    Serial.print("New action "); Serial.println(currentAction);
    SetupAction(g_context, currentAction);
  }

  if (!actionFinished && currentAction != ActionType::Terminate)
  {
    Step(g_context, currentAction);
  }

  if (g_context.showNeeded)
  {
    g_context.showNeeded = false;
    g_context.strip.show();
  }
}
