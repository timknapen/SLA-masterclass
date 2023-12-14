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
int ballRadius = 20;

// Timing
unsigned long lastFrame = 0;

// SHARP display
Adafruit_SharpMem display =
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, height, width, 8000000);

// Wii Nunchuck
WiiNunchuck chuck;

// RGB LED
Adafruit_NeoPixel pixels(1, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

#define NUM_OBSTACLES 20
TKPoint obstacles[NUM_OBSTACLES]; // we created a array of TKpoints
int obstacleRadius = 25;

void setupObstacles()
{
  for (int i = 0; i < NUM_OBSTACLES; i++)
  {
    obstacles[i].x = random(10, width - 10);
    obstacles[i].y = random(60, height - 10);
  }
}

// fade pixels
void pixelShow()
{
  // fade in blue
  for (int i = 0; i < 255; i++)
  {
    pixels.setPixelColor(0, pixels.Color(0, 0, i));
    pixels.show();
    delay(1);
  }
  // blue to red
  for (int i = 0; i < 255; i++)
  {
    pixels.setPixelColor(0, pixels.Color(i, 0, 255 - i));
    pixels.show();
    delay(1);
  }
  // fade out red
  for (int i = 0; i < 255; i++)
  {
    pixels.setPixelColor(0, pixels.Color(255 - i, 0, 0));
    pixels.show();
    delay(1);
  }
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
}

void setupDisplay()
{
  SPI.begin(PIN_LCD_CLK, -1, PIN_LCD_DI, PIN_LCD_CS);
  display.begin();
  display.setRotation(1);
  display.clearDisplayBuffer();
}

void boundsCheck()
{
  if (pos.x < ballRadius)
  {
    pos.x = ballRadius;
  }
  if (pos.x > width - ballRadius)
  {
    pos.x = width - ballRadius;
  }
  if (pos.y < ballRadius)
  {
    pos.y = ballRadius;
  }
  if (pos.y > height - ballRadius)
  {
    pos.y = height - ballRadius;
  }
}

void setup()
{
  Serial.begin(57600);
  delay(1000);
  Serial.println(" Wii Nunchuck test");
  delay(100);

  setupDisplay();

  setupObstacles();

  // RGB LED show
  pixels.begin();
  pixelShow();

  // setup nunchuck
  chuck.begin(PIN_SDA, PIN_SCL);

  // write something to display
  display.clearDisplayBuffer();
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(10, 200);
  display.println(" Dont get caught! ");
  display.refresh();
  delay(2000);
  display.clearDisplayBuffer();
}

void drawObstacle(int x, int y)
{
  int r = obstacleRadius;
  display.fillTriangle(x - r, y + r, // left
                       x, y - r,     // top
                       x + r, y + r, // right
                       BLACK);
}
// OBSTACLE IS A TRIANGLE

void drawObstacles()
{
  for (int i = 0; i < NUM_OBSTACLES; i++)
  {
    drawObstacle(obstacles[i].x, obstacles[i].y);
  }
}

void collideObstacles()
{
  // check all obstacles and see if we have a collision
  float r = obstacleRadius + ballRadius;
  for (int i = 0; i < NUM_OBSTACLES; i++)
  {
    // these coordinates define the box around the obstacle
    float x0 = obstacles[i].x - r;
    float y0 = obstacles[i].y - r;
    float x1 = obstacles[i].x + r;
    float y1 = obstacles[i].y + r;
    if (pos.x > x0 && pos.x < x1 && pos.y > y0 && pos.y < y1)
    {
      // if we are here, the ball is inside the box
      // just go back one step?
      // pos = pos - vel;
      float dx = pos.x - obstacles[i].x;
      float dy = pos.y - obstacles[i].y;

      float dist = sqrt(dx * dx + dy * dy);
      if (dist < r)
      {
        // bounce away!
        pos.x += dx / 2;
        pos.y += dy / 2;
      }
    }
  }
}

void drawEye(int x, int y, bool isOpen)
{
  // EYES
  if (isOpen)
  { // PUPILS
    display.fillCircle(x, y, 15, WHITE);

    display.fillCircle(x, y, 5, BLACK);
  }
  else
  {
    // closed eye

    display.drawFatLine((x)-15, y, (x) + 15, y, 3, BLACK);
  }
}

void loop()
{
  unsigned long now = millis();

  if (now > lastFrame + 1000 / 60)
  { // 60fps
    float frameTime = now - lastFrame;
    int FPS = 0;
    if (frameTime > 0)
    {
      FPS = 1000 / frameTime;
    }
    lastFrame = now; // reset the timer

    // Wii
    chuck.update(); // get data from nunchuck

    display.clearDisplayBuffer();

    display.fillRect(0, 0, width, 60, GRAY);

    bool isOpen = (millis() / 1000) % 6 < 3;
    drawEye(width / 3, 30, isOpen);
    drawEye(2 * width / 3, 30, isOpen);

    if (chuck.c_button)
    {
      // display.fillCircle(40, height / 4, 20, BLACK);
      setupObstacles();
    }
    if (chuck.z_button)
    {
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
    // display.fillCircle(ap.x, ap.y, 5, GRAY);

    // MAIN GUY
    vel.set(chuck.aX - 128, -(chuck.aY - 128));
    vel = vel * 0.1;
    pos = pos + vel;
    boundsCheck();
    collideObstacles();
    display.fillCircle(pos.x, pos.y, ballRadius, BLACK);

    // draw center crosshair
    // display.drawLine(0, height / 2, width, height / 2, BLACK);
    // display.drawLine(width / 2, 0, width / 2, height, BLACK);

    // send out to display

    drawObstacles();

    display.refresh();
  }
}
