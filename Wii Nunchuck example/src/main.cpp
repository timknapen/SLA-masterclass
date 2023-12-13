#include "TKPoint.h"
#include "WiiNunchuck.h"
#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SharpMem.h>
#include <Arduino.h>

// screen dimensions
const int height = 400;
const int width = 240;

TKPoint pos(width / 2, height);
TKPoint vel(0, 0);

// Timing
unsigned long lastFrame = 0;

// SHARP display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, height, width, 8000000);

// Wii Nunchuck
WiiNunchuck chuck;

// RGB LED
Adafruit_NeoPixel pixels(1, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

// OBSTACLES
#define NUM_OBSTACLES 5
TKPoint obstacles[NUM_OBSTACLES]; // we created an array of TKPoints

void setupObstacles() {
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    obstacles[i].x = random(10, width - 10);
    obstacles[i].y = random(10, height - 10);
  }
}

void drawObstacle(int x, int y) {
  // the obstacle is a triangle
  display.fillTriangle(x - 10, y, // left point
                       x, y - 10, // top point
                       x + 10, y, // right point
                       BLACK);
}

void drawObstacles() {
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    drawObstacle(obstacles[i].x, obstacles[i].y);
  }
}

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
  display.setRotation(1);
  display.clearDisplayBuffer();
}

void boundsCheck() {
  if (pos.x < 0) {
    pos.x = 0;
  }
  if (pos.x > width) {
    pos.x = width;
  }
  if (pos.y < 0) {
    pos.y = 0;
  }
  if (pos.y > height) {
    pos.y = height;
  }
}

void setup() {
  Serial.begin(57600);
  delay(1000);
  Serial.println(" Wii Nunchuck test");
  delay(100);

  setupDisplay();

  // RGB LED show
  pixels.begin();
  pixelShow();

  setupObstacles();

  // setup nunchuck
  chuck.begin(PIN_SDA, PIN_SCL);

  // write something to display
  display.clearDisplayBuffer();
  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.setCursor(0, 5);
  display.println(" Wii Nunchuck test");
  display.refresh();
  delay(1000);
  display.clearDisplayBuffer();
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

    // Wii
    chuck.update(); // get data from nunchuck

    display.clearDisplayBuffer();
    // minigolf hole
    display.fillCircle(width / 2, 30, 12, GRAY);

    if (chuck.c_button) {
      display.fillCircle(40, height / 4, 20, BLACK);
    }
    if (chuck.z_button) {
      display.fillCircle(40, 3 * height / 4, 20, BLACK);
    }

    // chuck.printRaw(); // for debugging!
    // TKPoint cp(2 * chuck.joyX, -2 * chuck.joyY);
    TKPoint cc(width / 2, height / 2);

    // draw accelerometer positions
    // display.fillRect(400 / 4, 240 / 2, 4, chuck.aX - 128, GRAY);
    // display.fillRect(400 / 2, 240 / 2, 4, chuck.aY - 128, GRAY);
    // display.fillRect(3 * 400 / 4, 240 / 2, 4, chuck.aZ - 128, GRAY);

    TKPoint ap(chuck.aX - 128, -(chuck.aY - 128));
    ap = ap + cc;
    // draw joystick position
    display.fillCircle(ap.x, ap.y, 5, GRAY);

    // MAIN GUY
    vel.set(chuck.aX - 128, -(chuck.aY - 128));
    vel = vel * 0.1;
    pos = pos + vel;
    boundsCheck();
    display.fillCircle(pos.x, pos.y, 10, BLACK);

    // draw center crosshair
    display.drawLine(0, height / 2, width, height / 2, BLACK);
    display.drawLine(width / 2, 0, width / 2, height, BLACK);

    // draw all obstacles
    drawObstacles();

    // send out to display
    display.refresh();
  }
}
