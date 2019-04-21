#define NO_INPUT 1

#include "Context.h"
#include "Color.h"
#include "Device.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"

#define TRACE 1

#define ACTIVATE_BUTTON_PIN 4
#define POWER_LED_PIN 5
#define POWER_LED_BRIGHTNESS 8

#define BRIGHTNESS_PIN 3
#define BRIGHTNESS_TOLERANCE 8

Button pushButton(ACTIVATE_BUTTON_PIN);

// The one and only global application context
Context g_context;

Device dev0(0, 20);
Device dev1(20, 20);

#define WAIT(D) ActionType::Wait, D
#define COLOR_1(D, FROM, TO) ActionType::Color1, D, FROM, TO
#define COLOR_2(D, FROM1, TO1, FROM2, TO2) ActionType::Color2, D, FROM1, TO1, FROM1, TO2
#define COLOR_TO_1(D, TO) ActionType::ColorTo1, D, TO
#define COLOR_TO_2(D, TO1, TO2) ActionType::ColorTo2, D, TO1, TO2

const static uint32_t actionData[] = {

  COLOR_2(100, BLACK, BLUE, BLACK, GREEN),
  COLOR_2(100, BLACK, GREEN, BLACK, BLUE),
  COLOR_2(100, BLACK, BLUE, BLACK, GREEN),
  COLOR_2(100, BLACK, GREEN, BLACK, BLUE),
  COLOR_2(100, BLACK, BLUE, BLACK, GREEN),
  COLOR_2(100, BLACK, GREEN, BLACK, BLUE),
  COLOR_2(100, BLACK, BLUE, BLACK, GREEN),
  COLOR_2(100, BLACK, GREEN, BLACK, BLUE),
  COLOR_2(100, BLACK, BLUE, BLACK, GREEN),
  COLOR_2(100, BLACK, GREEN, BLACK, BLUE),
  COLOR_2(100, BLACK, BLUE, BLACK, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_2(100, GREEN, BLUE),
  COLOR_TO_2(100, BLUE, GREEN),
  COLOR_TO_1(2000, BLACK),
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
byte brightness = 64;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pushButton.CheckState(g_context);
  g_context.strip.begin();
  g_context.strip.setBrightness(brightness);
  dev0.SetColor(g_context, 0);
  dev1.SetColor(g_context, 0);
  g_context.strip.show();
}

unsigned long _waitStart;
uint32_t _waitDuration;

void SetupAction(Context& context, ActionType action)
{
  switch (action)
  {
    case ActionType::Terminate:
      break;

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
    currentAction = GetNextAction();
    Serial.print("New action "); Serial.println(currentAction);
    SetupAction(g_context, currentAction);
    actionFinished = false;
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
