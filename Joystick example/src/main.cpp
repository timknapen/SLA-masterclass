
#include "Adafruit_seesaw.h"
#include "TKPoint.h"
#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SharpMem.h>
#include <Arduino.h>
#define TWI_FREQ 10000L

// screen dimensions
const int height = 240;
const int width = 400;

// Timing
unsigned long lastFrame = 0;

// SHARP display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, width, height, 8000000);

// can I create a second image buffer?
// GFX_canvas1 canvas

// Joystick
int joyX, joyY;
Adafruit_seesaw joystick;
uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
                       (1 << BUTTON_LEFT) | (1 << BUTTON_UP) |
                       (1 << BUTTON_SEL);

int lastX = 0;
int lastY = 0;

// setup the display
void setupDisplay() {
  SPI.begin(PIN_LCD_CLK, -1, PIN_LCD_DI, PIN_LCD_CS);
  display.begin();
  display.setRotation(0);
  display.clearDisplayBuffer();
}

// setup the joystick
void setupJoystick() {
  Wire.begin(PIN_SDA, PIN_SCL, TWI_FREQ);
  // Wire.begin(PIN_SDA, PIN_SCL);
  if (!joystick.begin(0x49)) {
    Serial.println("ERROR! joystick not found");
  } else {
    Serial.println("joystick started");
    Serial.print("version: ");
    Serial.println(joystick.getVersion(), HEX);
  }

  joystick.pinModeBulk(button_mask, INPUT_PULLUP);
}

// read the joystick
void readJoystick() {
  joyX = (joystick.analogRead(STICK_H) - STICK_CENTER_POINT);
  joyY = (joystick.analogRead(STICK_V) - STICK_CENTER_POINT);
}

//--------------------------------------------------
void setup() {
  Serial.begin((57600)); // start a serial port at 57600 BAUD
  delay(1000);           // wait one second aka 1000ms

  setupDisplay();
  setupJoystick();

  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.setCursor(5, 5);
  display.println(" A JOYSTICK");
  display.refresh(); // actually sends it to the display

  delay(1000);
}

//--------------------------------------------------
void loop() {
  unsigned long now = millis(); // get the current time

  if (now > lastFrame + 1000 / 30) { // do this every 60fps
    lastFrame = now;
    readJoystick();

    display.clearDisplayBuffer(); // clear the screen

    display.fillCircle(width / 2 + joyX / 4, height / 2 + joyY / 4, 10, GRAY);

    display.drawFatLine(
        width / 2 + joyX / 4, height / 2 + joyY / 4,   // new position
        width / 2 + lastX / 4, height / 2 + lastY / 4, // old position
        10, BLACK);
    lastX = joyX;
    lastY = joyY;

    // draw center crosshair
    display.drawLine(0, height / 2, width, height / 2, BLACK);
    display.drawLine(width / 2, 0, width / 2, height, BLACK);

    // rotating line
    TKPoint pt(0, 100);
    TKPoint center(width / 2, height / 2);
    pt.rotate(360.0f * (float)millis() / 20000.0f);
    pt += center;
    display.drawFatLine(center.x, center.y, pt.x, pt.y, 5, BLACK);

    display.refresh(); // actually sends it to the display
  }
}
