#include <Adafruit_NeoPixel.h>
#define TRACE 1
#define WATCHDOG_TIMEOUT_SECONDS 20

#define ACTIVATE_BUTTON_PIN 3
#define BRIGHTNESS_PIN 3
#define BRIGHTNESS_TOLERANCE 8

// NeoPixel definitions
#define LED_PIN    6
#define LED_COUNT 40

#include "Context.h"
#include "Color.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"
#include "ExtrapolateAction.h"

// The one and only global application context
Context g_context;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Button pushButton(ACTIVATE_BUTTON_PIN);
AnalogReader reader(BRIGHTNESS_PIN, BRIGHTNESS_TOLERANCE);
Watchdog watchdog(WATCHDOG_TIMEOUT_SECONDS * 1000);

Action* actions[] = {
  new ExtrapolateAction(BLACK, YELLOW, 1000, &strip),
  //new WaitForButton(&pushButton),
  new WaitAction(2000), // because pushbutton died
  new ExtrapolateAction(BLACK, RED, 1500, &strip),
  new ExtrapolateAction(BLACK, GREEN, 1500, &strip),
  new ExtrapolateAction(BLACK, BLUE, 1500, &strip),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 500, &strip),
  new ExtrapolateAction(WHITE, BLACK, 0, &strip),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 50, &strip),
  new ExtrapolateAction(BLACK, WHITE, 50, &strip),
  new ExtrapolateAction(BLACK, WHITE, 1000, &strip),
  new WaitAction(100),
  new TerminateAction(&strip),
};

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

  strip.begin();
  strip.setBrightness(brightness);
  strip.show();
}

void loop()
{
  g_context.now = millis();

  auto pushButtonChanged = pushButton.CheckState(g_context);
  auto brightnessChanged = reader.CheckState(g_context);

  if (watchdog.IsTimeout(g_context))
  {
    Action::setAll(g_context, &strip, BLACK);
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
    Action::setAll(g_context, &strip, WHITE);
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
    strip.setBrightness(brightness);
    Action::setAll(g_context, &strip, g_context.lastColor.color);
    watchdog.Pat(g_context);
  }

  if (g_context.showNeeded)
  {
    g_context.showNeeded = false;
    strip.show();
  }
}
