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
AnalogReader reader(3, 8);
Watchdog watchdog(20 * 1000);

Action* actions[] = {
  new ExtrapolateAction(BLACK, YELLOW, 1000, &strip),
  new WaitForButton(&pushButton),
  new ExtrapolateAction(YELLOW, RED, 1500, &strip),
  new ExtrapolateAction(RED, GREEN, 1500, &strip),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 500, &strip),
  new ExtrapolateAction(WHITE, BLACK, 0, &strip),
  new WaitAction(1000),
  new ExtrapolateAction(BLACK, WHITE, 50, &strip),
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

void loop() {
   g_context.now = millis();
  
  auto pushButtonChanged = pushButton.CheckState(g_context);
  auto brightnessChanged = reader.CheckState(g_context);
  
  if (watchdog.IsTimeout(g_context))
  {
    setAll(g_context, &strip, BLACK);
    strip.show();
    actionIndex = -1;
    actionFinished = false;
    return;
  }

  if (brightnessChanged)
  {
    brightness = 255 - reader.GetValue() / 4;
    Serial.print("Brightness "); Serial.println(brightness);
    strip.setBrightness(brightness);
    setAll(g_context, &strip, g_context.lastColor.color);
    strip.show();
    watchdog.Pat(g_context);
  }

  if (isSetupMode)
  {
    if (pushButton.IsPressed())
    {
      isSetupMode = false;      
    }
    setAll(g_context, &strip, WHITE);
    strip.show();
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
