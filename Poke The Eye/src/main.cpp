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

int ballRadius = 20;
TKPoint pos(width / 2, height - ballRadius);
TKPoint vel(0, 0);
float movement = 0;

void trackMovement() {
  float frameMovement = vel.length() / 128.0f;
  movement -= 0.1; // automatically decrease movement!

  if (movement < 0) {
    movement = 0;
  }
  if (movement > 10) {
    movement = 10;
  }
}

void drawMovement() {
  display.fillRect(width - 20, height / 2, 20, 50, GRAY);
  display.fillRect(width - 20, height / 2 + 50 - movement * 5, 20, movement * 5,
                   GRAY);
}

// Timing
unsigned long lastFrame = 0;

// SHARP display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, height, width, 8000000);

// Wii Nunchuck
WiiNunchuck chuck;

// RGB LED
Adafruit_NeoPixel pixels(1, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

#define NUM_OBSTACLES 10
TKPoint obstacles[NUM_OBSTACLES]; // we created a array of TKpoints
int obstacleRadius = 20;

void setupObstacles() {
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    obstacles[i].x = random(obstacleRadius, width - obstacleRadius);
    obstacles[i].y = random(60, height - 60);
  }
}

void setupObstaclesInGrid(int gridBox) {
  int numW = ceil(width - 2 * obstacleRadius) / gridBox;
  int numH = ceil(height - 2 * 60) / gridBox;
  for (int i = 0; i < NUM_OBSTACLES; i++) {
    obstacles[i].x = obstacleRadius + random(0, numW + 1) * gridBox;
    obstacles[i].y = 60 + obstacleRadius + random(0, numH + 1) * gridBox;
  }
}

void drawObstacle(int x, int y) {
  int r = 1.5 * obstacleRadius;
  display.fillTriangle(x - r, y + r, // left
                       x, y - r,     // top
                       x + r, y + r, // right
                       BLACK);
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
        pos.x += dx / 10;
        pos.y += dy / 10;
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
  setupObstacles();

  // RGB LED show
  pixels.begin();
  // pixelShow();

  // setup nunchuck
  chuck.begin(PIN_SDA, PIN_SCL);

  // write something to display
  display.clearDisplayBuffer();
  display.setTextSize(6);
  display.setTextColor(BLACK);
  display.setCursor(0, 20);
  display.println("  POKE");
  display.println("  THE ");
  display.println("  EYE ");

  display.refresh();
  delay(2000);
  display.clearDisplayBuffer();
}

void drawEye(int x, int y, bool isOpen) {
  if (pos.y < 60) {
    // draw cross
    int r = 15;
    display.drawFatLine(x - r, y - r, x + r, y + r, 3, BLACK);
    display.drawFatLine(x - r, y + r, x + r, y - r, 3, BLACK);

    return;
  }
  if (isOpen) {
    // calculate where to look
    TKPoint eyePos(x, y);
    TKPoint dir = pos - eyePos;
    dir.normalize();
    TKPoint pupil = eyePos + dir * 16;

    display.fillCircle(x, y, 20, WHITE);            // EYES
    display.fillCircle(pupil.x, pupil.y, 8, BLACK); // PUPILS
  } else {
    // closed eye
    display.drawFatLine(x - 20, y, x + 20, y, 3, BLACK);
  }
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

    // Eyes and background
    display.fillRect(0, 0, width, 60, GRAY);
    bool isOpen = (millis() / 1000) % 6 < 3;
    drawEye(width / 3, 30, isOpen);
    drawEye(2 * width / 3, 30, isOpen);

    // start zone
    display.fillRect(0, height - 60, width, 60, GRAY);

    if (chuck.c_button) {
      setupObstaclesInGrid(1.5 * 2 * obstacleRadius);
      pos.set(width / 2, height - ballRadius);
    }
    if (chuck.z_button) {
      setupObstacles();
      pos.set(width / 2, height - ballRadius);
    }

    // chuck.printRaw(); // for debugging!


    // MAIN GUY
    vel.set(chuck.aX - 128, -(chuck.aY - 128));
    vel = vel * 0.3; // scale down
    pos = pos + vel;
    boundsCheck();
    collideObstacles();
    display.fillCircle(pos.x, pos.y, ballRadius, BLACK);

    drawObstacles();

     // track movement
    trackMovement();
    drawMovement();

    // send out to display
    display.refresh();
  }
}
