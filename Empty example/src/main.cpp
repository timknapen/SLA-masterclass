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

// SHARP display named display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, width, height, 8000000);

// setup the display
void setupDisplay() {
  SPI.begin(PIN_LCD_CLK, -1, PIN_LCD_DI, PIN_LCD_CS);
  display.begin();
  display.setRotation(0);
  display.clearDisplay();
}

//--------------------------------------------------
void setup() {
  Serial.begin((57600));                // start a serial port at 57600 BAUD
  delay(1000);                          // wait one second aka 1000ms
  Serial.println(" A Boolean Handaxe"); // send a message over serial

  setupDisplay();

  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.setCursor(5, 5);
  display.println(" IT WORKS!!!!");
  display.refresh(); // actually sends it to the display

  delay(1000);
  display.clearDisplay();
}

//--------------------------------------------------
void loop() {

  unsigned long now = millis(); // get the current time

  if (now > lastFrame + 1000 / 60) { // do this every 60fps
    lastFrame = now;
    display.clearDisplayBuffer();
    int x = (now / 10) % 400;
    int y = height / 2 + 100 * sin((float)now / 400);
    display.fillCircle(x, y, 10, BLACK);
    display.refresh(); // actually sends it to the display
  }
}
