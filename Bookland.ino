#include <Adafruit_NeoPixel.h>

#define NO_INPUT 1

#include "Context.h"
#include "Color.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"
#include "ExtrapolateAction.h"

#define TRACE 1
#define WATCHDOG_TIMEOUT_SECONDS 20

#define OPEN_SWITCH_PIN 3
#define ACTIVATE_BUTTON_PIN 4
#define POWER_LED_PIN 5
#define POWER_LED_BRIGHTNESS 8

#define BRIGHTNESS_PIN 3
#define BRIGHTNESS_TOLERANCE 8

// The one and only global application context
Context g_context;


Button pushButton(ACTIVATE_BUTTON_PIN);

Action* actions[] = {
  new ExtrapolateAction(BLACK, YELLOW, 1000),
#if NO_INPUT
  new WaitAction(2000), // because pushbutton died
#else
  new WaitForButton(&pushButton, true),
#endif
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

int actionIndex = -1;
bool actionFinished = false;
Action* pAction;
bool isSetupMode = false;
RGB color;
byte brightness = 255;
bool isClosed = true;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pushButton.CheckState(g_context);

  isSetupMode = pushButton.GetUndebouncedValue() == 1;

  Serial.print("Debug "); Serial.println(isSetupMode);

  //brightness = 255 - reader.GetValue() / 4;

  g_context.strip.begin();
  g_context.strip.setBrightness(brightness);
  Action::setAll(g_context, BLACK);
  g_context.strip.show();
}

void loop()
{
  g_context.now = millis();

  auto pushButtonChanged = pushButton.CheckState(g_context);

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

  if (g_context.showNeeded)
  {
    g_context.showNeeded = false;
    g_context.strip.show();
  }
}
