#include <Adafruit_NeoPixel.h>

#include "Context.h"
#include "Color.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"
#include "ExtrapolateAction.h"

// The one and only global application context
Context g_context;

typedef uint32_t pfnColorSelector(int);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Button pushButton(3);
AnalogReader reader(3);
Watchdog watchdog(5 * 1000);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pushButton.SetWatchdog(&watchdog);
  reader.SetWatchdog(&watchdog);

  strip.begin();
  strip.setBrightness(16);
  strip.show();
}

void buttonPressed(int value)
{
  if (value)
    Serial.println("Button pressed");
  else
    Serial.println("Button released");
}

Action* actions[] = {
  new ExtrapolateAction(BLACK, YELLOW, 1000, &strip),
  new WaitForButton(&pushButton),
  new ExtrapolateAction(YELLOW, RED, 1500, &strip),
  new ExtrapolateAction(RED, GREEN, 1500, &strip),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 500, &strip),
  new ExtrapolateAction(WHITE, BLACK, 0, &strip),
  new WaitAction(2000),
  new ExtrapolateAction(BLACK, WHITE, 500, &strip),
  new WaitAction(2000),
  new TerminateAction(&strip),
};

int actionIndex = -1;
bool actionFinished = false;
Action* pAction;

void loop() {

  g_context.now = millis();
  
  pushButton.CheckState(g_context);

  if (watchdog.IsTimeout(g_context))
  {
    setAll(&strip, BLACK);
    actionIndex = -1;
    actionFinished = false;
    return;
  }
  
  if (actionIndex == -1 || actionFinished)
  {
    actionIndex = (actionIndex + 1) % (sizeof(actions) / sizeof(Action*));
    Serial.print("Action "); Serial.println(actionIndex);
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
