#include "TKPoint.h"
#include "WiiNunchuck.h"
#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SharpMem.h>
#include <Arduino.h>

// screen dimensions
const int height = 240;
const int width = 400;

// Timing
unsigned long lastFrame = 0;

// SHARP display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, 400, 240, 8000000);

// Wii Nunchuck
WiiNunchuck chuck;

// RGB LED
Adafruit_NeoPixel pixels(1, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

// fade pixels
void pixelShow() {
  // fade in blue
  for (int i = 0; i < 255; i++) {
    pixels.setPixelColor(0, pixels.Color(0, 0, i));
    pixels.show();
    delay(1);
  }
  // blue to red
  for (int i = 0; i < 255; i++) {
    pixels.setPixelColor(0, pixels.Color(i, 0, 255 - i));
    pixels.show();
    delay(1);
  }
  // fade out red
  for (int i = 0; i < 255; i++) {
    pixels.setPixelColor(0, pixels.Color(255 - i, 0, 0));
    pixels.show();
    delay(1);
  }
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
}

void setupDisplay() {
  SPI.begin(PIN_LCD_CLK, -1, PIN_LCD_DI, PIN_LCD_CS);
  display.begin();
  display.setRotation(0);
  display.clearDisplay();
}

void setup() {
  Serial.begin(57600);
  delay(1000);
  Serial.println(" Wii Nunchuck test");
  delay(100);

  setupDisplay();

  pixels.begin();
  pixelShow();

  // setup nunchuck
  chuck.begin(PIN_SDA, PIN_SCL);

  // write something to display
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.setCursor(0, 5);
  display.println(" Wii Nunchuck test");
  display.refresh();
  delay(1000);
  display.clearDisplay();
}

void loop() {
  unsigned long now = millis();

  if (now > lastFrame + 1000 / 60) { // 60fps
    float frameTime = now - lastFrame;
    int FPS = 0;
    if (frameTime > 0) {
      FPS = 1000 / frameTime;
    }

    lastFrame = now; // reset the timer
    chuck.update();  // get data from nunchuck
    if (chuck.c_button) {
      display.clearDisplay();
      display.fillCircle(40, height / 4, 20, BLACK);
    }
    if (chuck.z_button) {
      display.fillCircle(40, 3 * height / 4, 20, BLACK);
    }

    // chuck.printRaw(); // for debugging!

    TKPoint cp(2 * chuck.joyX, -2 * chuck.joyY);
    TKPoint cc(width / 2, height / 2);
    cp = cp + cc;
    // display.clearDisplay();  // clear the display

    // draw accelerometer positions
    display.fillRect(400 / 4, 240 / 2, 4, chuck.aX - 128, GRAY);
    display.fillRect(400 / 2, 240 / 2, 4, chuck.aY - 128, GRAY);
    display.fillRect(3 * 400 / 4, 240 / 2, 4, chuck.aZ - 128, GRAY);

    // draw joystick position
    display.fillCircle(cp.x, cp.y, 5, BLACK);

    // draw center crosshair
    display.drawLine(0, height / 2, width, height / 2, BLACK);
    display.drawLine(width / 2, 0, width / 2, height, BLACK);

    // send out to display
    display.refresh();
  }
}
