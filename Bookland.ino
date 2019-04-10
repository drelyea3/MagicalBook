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
  strip.setBrightness(32);
  strip.show();
}

void buttonPressed(int value)
{
  if (value)
    Serial.println("Button pressed");
  else
    Serial.println("Button released");
}


void loop() {

  ExtrapolateAction fadeIn(RED, BLUE, 2000, &strip);

  fadeIn.Setup();
  while (fadeIn.Step());

  //  RGB from = {BLACK};
  //  RGB to = {COOL};
  //
  //  int deltaR = to.r - from.r;
  //  int deltaG = to.g - from.g;
  //  int deltaB = to.b - from.b;
  //
  //  auto start = millis();
  //  int duration = 1;
  //
  //  strip.setBrightness(64);
  //  while (true)
  //  {
  //    auto elapsed = millis() - start;
  //
  //    if (elapsed > duration)
  //    {
  //      setAll(to.color);
  //      break;
  //    }
  //
  //    RGB c;
  //    c.r = (elapsed * deltaR) / duration;
  //    c.g = (elapsed * deltaG) / duration;
  //    c.b = (elapsed * deltaB) / duration;
  //
  //    setAll(c.color);
  //
  //    if (pushButton.GetValue() == 1)
  //    {
  //      Serial.println("breaking");
  //      break;
  //    }
  //  }



  setAll(BLACK);
  
  while(true);  
      
//  
//
//
//
//  auto token = pushButton.AddHandler([](int n) {
//    Serial.println(n);
//  });
//  while (true)
//  {
//    auto buttonChanged = pushButton.CheckState();
//    if (buttonChanged)
//    {
//      //      Serial.print("changed ");
//      //      Serial.println(pushButton.GetValue());
//    }
//
//    auto dialChanged = reader.CheckState();
//    if (dialChanged)
//    {
//      Serial.print("reader changed ");
//      Serial.println(reader.GetValue());
//    }
//
//    if (watchdog.IsTimeout())
//    {
//      Serial.println("WATCHDOG TIMEOUT");
//      pushButton.RemoveHandler(token);
//    }
//  }
//
//  selectColor([] (int n)
//  {
//    return (n / 8 + n) % 2 ? RED : BLUE;
//  });
//
//  auto button = digitalRead(3);
//  while (button == 1)
//  {
//    auto control = analogRead(3);
//    Serial.println(control);
//    auto brightness = control / 4;
//    Serial.println(brightness);
//    selectColor([] (int n)
//    {
//      return (n / 8 + n) % 2 ? RED : BLUE;
//    });
//    strip.setBrightness(brightness);
//    strip.show();
//    button = digitalRead(3);
//  }
//  setAll(BLACK);
//  while (true) {}
//  while (true)
//  {
//    auto button = digitalRead(3);
//    //Serial.println(button);
//    auto control = analogRead(3);
//    Serial.println(control);
//    selectColor([] (int n)
//    {
//      return (n / 8 + n) % 2 ? RED : BLUE;
//    });
//
//    delay(500);
//
//    selectColor([] (int n)
//    {
//      return (n / 8 + n) % 2 ? BLUE : RED;
//    });
//
//    delay(500);
//  }
//
//  Serial.println("loop");
//  auto stop = millis();
//  transition(BLACK, BLUE, 500, 0);
//  transition(BLUE, BLACK, 500, 0);
//  auto elapsed = millis() - stop;
//  Serial.println(elapsed);
//
//  transition(BLACK, RED, 500, 0);
//  transition(RED, BLACK, 500, 0);
//
//  transition(BLACK, YELLOW, 500, 0);
//  transition(YELLOW, BLACK, 500, 0);
//
//  transition(BLACK, WHITE, 50, 0);
//  transition(WHITE, BLACK, 2000, 0);
//  //while(1)
  delay(2000);
}

void setAll(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

//void transition(uint32_t color_from, uint32_t color_to, int step_count, int step_delay)
//{
//  float deltaR = (float)(_R(color_to) - _R(color_from)) / (float)step_count;
//  float deltaG = (float)(_G(color_to) - _G(color_from)) / (float)step_count;
//  float deltaB = (float)(_B(color_to) - _B(color_from)) / (float)step_count;
//  /*
//    Serial.println(_R(color_to));
//    Serial.println(_G(color_to));
//    Serial.println(_B(color_to));
//  */
//  float r = (float)_R(color_from);
//  float g = (float)_G(color_from);
//  float b = (float)_B(color_from);
//  /*
//    Serial.println(deltaR);
//    Serial.println(deltaG);
//    Serial.println(deltaB);
//  */
//  for (int step = 0; step < step_count; ++step)
//  {
//    uint32_t c = _RGB((byte)r, (byte)g, (byte)b);
//    /*    Serial.print("c = ");
//        Serial.println(c, HEX);
//    */
//    setAll(c);
//    delay(step_delay);
//    r += deltaR;
//    g += deltaG;
//    b += deltaB;
//  }
//}
//
//void colorWipe(uint32_t color, int wait) {
//  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
//    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//    strip.show();                          //  Update strip to match
//    delay(wait);                           //  Pause for a moment
//  }
//}
//
//void selectColor(pfnColorSelector fn)
//{
//  auto max = strip.numPixels();
//  for (int i = 0; i < max; ++i)
//  {
//    strip.setPixelColor(i, fn(i));
//  }
//  strip.show();
//}
