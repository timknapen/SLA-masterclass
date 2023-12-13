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
int ballRadius = 10;

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
#define NUM_OBSTACLES 10
TKPoint obstacles[NUM_OBSTACLES]; // we created an array of TKPoints
int obstacleRadius = 20;

void setupObstacles() {
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    obstacles[i].x = random(10, width - 10);
    obstacles[i].y = random(10, height - 10);
  }
}

void drawObstacle(int x, int y) {
  // the obstacle is a triangle
  int r = obstacleRadius;
  // display.fillTriangle(x - r, y + r, // left point
  //                      x, y - r,     // top point
  //                      x + r, y + r, // right point
  //                      GRAY);

  // the obstacle is a circle
  display.fillCircle(x, y, r, BLACK);
  // the obstacle is a square
  // display.fillRect(x - r, y - r, 2 * r, 2 * r, BLACK);
}

void drawObstacles() {
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    drawObstacle(obstacles[i].x, obstacles[i].y);
  }
}

void collideObstacles() {
  // check all obstacles and see if we have a collision
  float r = obstacleRadius + ballRadius;
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    // these coordinates define the box around the obstacle
    float x0 = obstacles[i].x - r;
    float y0 = obstacles[i].y - r;
    float x1 = obstacles[i].x + r;
    float y1 = obstacles[i].y + r;
    if (pos.x > x0 && pos.x < x1 && pos.y > y0 && pos.y < y1) {
      // if we are here, the ball is inside the box
      // just go back one step?
      // pos = pos - vel;
      float dx = pos.x - obstacles[i].x;
      float dy = pos.y - obstacles[i].y;

      float dist = sqrt(dx * dx + dy * dy);
      if (dist < r) {
        // bounce away!
        pos.x += dx / 2;
        pos.y += dy / 2;
      }
    }
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
  if (pos.x < ballRadius) {
    pos.x = ballRadius;
  }
  if (pos.x > width - ballRadius) {
    pos.x = width - ballRadius;
  }
  if (pos.y < ballRadius) {
    pos.y = ballRadius;
  }
  if (pos.y > height - ballRadius) {
    pos.y = height - ballRadius;
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
      // display.fillCircle(40, height / 4, 20, 3);
      setupObstacles();
    }
    if (chuck.z_button) {
      // display.fillCircle(40, 3 * height / 4, 20, 3);
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
    // display.fillCircle(ap.x, ap.y, 5, GRAY);

    // MAIN GUY MOVEMENT
    vel.set(chuck.aX - 128, -(chuck.aY - 128));
    vel = vel * 0.1;
    pos = pos + vel;
    boundsCheck();
    collideObstacles();
    display.fillCircle(pos.x, pos.y, ballRadius, BLACK);

    // draw center crosshair
    // display.drawLine(0, height / 2, width, height / 2, BLACK);
    // display.drawLine(width / 2, 0, width / 2, height, BLACK);

    // draw all obstacles
    drawObstacles();

    // send out to display
    display.refresh();
  }
}
