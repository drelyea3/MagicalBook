#define NO_INPUT 1

#include "Context.h"
#include "Color.h"
#include "Watchdog.h"
#include "Button.h"
#include "AnalogReader.h"
#include "Action.h"
#include "ExtrapolateAction.h"

#define TRACE 1

#define ACTIVATE_BUTTON_PIN 4
#define POWER_LED_PIN 5
#define POWER_LED_BRIGHTNESS 8

#define BRIGHTNESS_PIN 3
#define BRIGHTNESS_TOLERANCE 8

Button pushButton(ACTIVATE_BUTTON_PIN);

// The one and only global application context
Context g_context;

Device matrix;

auto goWhite = new DeviceExtrapolateAction(&matrix, BLACK, WHITE, 20);
auto goBlack = new DeviceExtrapolateAction(&matrix, WHITE, BLACK, 20);

Action* actions[] = {
  new DeviceExtrapolateAction(&matrix, BLACK, YELLOW, 1000),
  new WaitAction(1000), // because pushbutton died
  new DeviceExtrapolateAction(&matrix, RED, BLACK, 1000),
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 1000),
  new DeviceExtrapolateAction(&matrix, BLUE, BLACK, 1000),
  new DeviceExtrapolateAction(&matrix, RED, BLACK, 500),
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 500),
  new DeviceExtrapolateAction(&matrix, BLUE, BLACK, 500),
  new DeviceExtrapolateAction(&matrix, RED, BLACK, 200),
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 200),
  new DeviceExtrapolateAction(&matrix, BLUE, BLACK, 200),
  new DeviceExtrapolateAction(&matrix, RED, BLACK, 100),
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 100),
  new DeviceExtrapolateAction(&matrix, BLUE, BLACK, 100),
  new DeviceExtrapolateAction(&matrix, RED, BLACK, 50),
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 50),
  new DeviceExtrapolateAction(&matrix, BLUE, BLACK, 50),
  new DeviceExtrapolateAction(&matrix, RED, BLACK, 20),
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 20),
  new DeviceExtrapolateAction(&matrix, BLUE, BLACK, 20),
  goWhite, goBlack,
  goWhite, goBlack,
  goWhite, goBlack,
  goWhite, goBlack,
  goWhite, goBlack,
  new DeviceExtrapolateAction(&matrix, YELLOW, BLACK, 1000),
  new TerminateAction(),
};

int actionIndex = -1;
bool actionFinished = false;
Action* pAction;
RGB color;
byte brightness = 16;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  pushButton.CheckState(g_context);
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
