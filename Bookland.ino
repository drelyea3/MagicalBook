#include <Adafruit_NeoPixel.h>

#include "Context.h"
#include "Color.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"
#include "ExtrapolateAction.h"

#define TRACE 1
#define WATCHDOG_TIMEOUT_SECONDS 20

#define ACTIVATE_BUTTON_PIN 3
#define BRIGHTNESS_PIN 3
#define BRIGHTNESS_TOLERANCE 8

// The one and only global application context
Context g_context;

Action* actions[] = {
  new ExtrapolateAction(BLACK, YELLOW, 1000),
  //new WaitForButton(&pushButton),
  new WaitAction(2000), // because pushbutton died
  new ExtrapolateAction(BLACK, RED, 1500),
  new ExtrapolateAction(BLACK, GREEN, 1500),
  new ExtrapolateAction(BLACK, BLUE, 1500),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 500),
  new ExtrapolateAction(WHITE, BLACK, 0),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 50),
  new ExtrapolateAction(BLACK, WHITE, 50),
  new ExtrapolateAction(BLACK, WHITE, 1000),
  new WaitAction(100),
  new TerminateAction(),
};

Button pushButton(ACTIVATE_BUTTON_PIN);
AnalogReader reader(BRIGHTNESS_PIN, BRIGHTNESS_TOLERANCE);
Watchdog watchdog(WATCHDOG_TIMEOUT_SECONDS * 1000);

int actionIndex = -1;
bool actionFinished = false;
Action* pAction;
bool isSetupMode = false;
RGB color;
byte brightness;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pushButton.SetWatchdog(&watchdog);

  reader.CheckState(g_context);
  pushButton.CheckState(g_context);

  isSetupMode = pushButton.GetUndebouncedValue() == 1;

  Serial.print("Debug "); Serial.println(isSetupMode);

  brightness = 255 - reader.GetValue() / 4;

  g_context.strip.begin();
  g_context.strip.setBrightness(brightness);
  g_context.strip.show();
}

void loop()
{
  g_context.now = millis();

  auto pushButtonChanged = pushButton.CheckState(g_context);
  auto brightnessChanged = reader.CheckState(g_context);

  if (watchdog.IsTimeout(g_context))
  {
    Action::setAll(g_context, BLACK);
    actionIndex = -1;
    actionFinished = false;
    delay(1000);
  }
  else if (isSetupMode)
  {
    if (pushButton.IsPressed())
    {
      isSetupMode = false;
    }
    Action::setAll(g_context, WHITE);
  }
  else // normal action processing
  {
    if (actionIndex == -1 || actionFinished)
    {
      actionIndex = (actionIndex + 1) % (sizeof(actions) / sizeof(Action*));
#if TRACE
      Serial.print("Action "); Serial.println(actionIndex);
#endif
      pAction = actions[actionIndex];
      actionFinished = false;
      pAction->Setup(g_context);
    }

    if (pAction)
    {
      actionFinished = !pAction->Step(g_context);
    }

    if (actionFinished)
    {
      pAction->Teardown(g_context);
    }
  }

  // always respond to brightness changes
  if (brightnessChanged)
  {
    brightness = 255 - reader.GetValue() / 4;
    Serial.print("Brightness "); Serial.println(brightness);
    g_context.strip.setBrightness(brightness);
    Action::setAll(g_context, g_context.lastColor.color);
    watchdog.Pat(g_context);
  }

  if (g_context.showNeeded)
  {
    g_context.showNeeded = false;
    g_context.strip.show();
  }
}
