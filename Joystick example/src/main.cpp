
#include "Adafruit_Thermal.h" // PRINTER!
#include "Adafruit_seesaw.h"
#include "Canvas.h"
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
Canvas canvas = Canvas(width, height);
bool bDrawCanvas = true;
Adafruit_Thermal printer(&Serial1); // Pass addr to printer constructor

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
  canvas.clear();
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

// PRINTER
void setupPrinter() {
  Serial1.begin(19200, SERIAL_8N1, PIN_PRINTER_RX, PIN_PRINTER_TX);
  printer.begin();
}

void printImage() {
  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults

  canvas.invert();
  printer.printBitmap(canvas.getWidth(), canvas.getHeight(),
                      canvas.getBuffer());
  canvas.invert();

  printer.feed(5);

  printer.sleep(); // Tell printer to sleep
}

//--------------------------------------------------
void setup() {
  Serial.begin((57600)); // start a serial port at 57600 BAUD
  delay(1000);           // wait one second aka 1000ms

  setupDisplay();
  setupJoystick();
  setupPrinter();

  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.setCursor(5, 5);
  display.println(" A JOYSTICK");
  display.refresh(); // actually sends it to the display

  delay(1000);
}

void readSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
    case 'c':
      bDrawCanvas = !bDrawCanvas;
      Serial.println(bDrawCanvas ? " Draw canvas" : " Do not draw canvas");
      break;
    case 'x':
      canvas.clear();
      Serial.println("Clear canvas");
      break;
    case 'p':
      printImage();
      break;
    }
  }
}

//--------------------------------------------------
void loop() {
  unsigned long now = millis(); // get the current time
  readSerial();
  if (now > lastFrame + 1000 / 30) { // do this every 60fps
    lastFrame = now;
    readJoystick();

    display.clearDisplayBuffer(); // clear the screen

    // The joystick drawing
    // canvas.fillCircle(width / 2 + joyX / 4, height / 2 + joyY / 4, 10, GRAY);
    // canvas.drawFatLine(
    //     width / 2 + joyX / 4, height / 2 + joyY / 4,   // new position
    //     width / 2 + lastX / 4, height / 2 + lastY / 4, // old position
    //     10, BLACK);
    // lastX = joyX;
    // lastY = joyY;

    // DEMO Rotating line in the center
    float l = sinf((float)millis() / 230.0f);
    TKPoint pt(0, 50 + 40 * l);
    TKPoint center(width / 2, height / 2);
    pt.rotate(360.0f * (float)millis() / 2300.0f);
    pt += center;
    canvas.drawFatLine(lastX, lastY, pt.x, pt.y, 5 + 0.5, BLACK);
    canvas.fillCircle(pt.x, pt.y, 5, BLACK);

    lastX = pt.x;
    lastY = pt.y;
    display.fillCircle(pt.x, pt.y, 10, GRAY);

    if (bDrawCanvas) {
      canvas.invert();
      // drawBitmap inverts!
      display.drawBitmap(0, 0, canvas.getBuffer(), width, height, BLACK);
      canvas.invert();
    }

    // Draw the grid
    int gridw = 20;
    for (int x = 0; x < width; x += gridw) {
      display.drawLine(x, 0, x, height, BLACK);
    }
    for (int y = 0; y < height; y += gridw) {
      display.drawLine(0, y, width, y, BLACK);
    }

    display.refresh(); // actually sends it to the display
  }
}
