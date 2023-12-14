#include "Adafruit_VL53L0X.h"
#include "TKPoint.h"
#include "pins.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SharpMem.h>
#include <Arduino.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// screen dimensions
const int height = 240;
const int width = 400;

// Timing
unsigned long lastFrame = 0;

// Measurement
int distance = 0;

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

// zoom function
float interpolate(float start, float end, float pct) {
  return start + pct * (end - start);
}

float interpolateFloat(float start, float end, float pct) {
  // pct is a value between 0  and 1
  return start + pct * (end - start);
}

TKPoint interpolate(TKPoint start, TKPoint end, float pct) {
  // pct is a value between 0  and 1
  return start + ((end - start) * pct);
}

float findZoom(float start, float end, float pos) {
  // finds the zoom level within our scene
  if (end == start) {
    return 1;
  }
  return (pos - start) / (end - start);
}

void drawHuman() {

  TKPoint eye1(195, 95, 2);   // positie en radius van oog op zoom 0
  TKPoint eye2(10, 20, 50); // positie en radius van oog op zoom 100

  // finds the zoom level within our scene
  float zoomLevel = findZoom(300, 200, distance);
  
  TKPoint zoomedEye = interpolate(eye1, eye2, zoomLevel);

  // Draw ground
  display.fillRect(0, 195, 400, 60, BLACK);

  // Draw sun
  display.fillCircle(50, 50, 30, BLACK);

  // Draw cacti
  display.fillRect(250, 135, 10, 60, BLACK);

  // Add branches to the cactus (right side)
  display.drawLine(255, 150, 265, 160, BLACK); // Right branch
  display.drawLine(265, 160, 255, 170, BLACK);
  display.fillRect(260, 155, 5, 5, BLACK); // Fill the right branch

  // Add branches to the cactus (left side)
  display.drawLine(245, 150, 255, 160, BLACK); // Left branch
  display.drawLine(255, 160, 245, 170, BLACK);
  display.fillRect(250, 155, 5, 5, BLACK); // Fill the left branch

  display.fillRect(270, 115, 10, 80, BLACK);

  // Add branches to the cactus (right side)
  display.drawLine(275, 150, 285, 160, BLACK); // Right branch
  display.drawLine(285, 160, 275, 170, BLACK);
  display.fillRect(280, 155, 5, 5, BLACK); // Fill the right branch

  // Add branches to the cactus (left side)
  display.drawLine(270, 150, 280, 160, BLACK); // Left branch
  display.drawLine(280, 160, 270, 170, BLACK);
  display.fillRect(275, 155, 5, 5, BLACK); // Fill the left branch

  display.fillRect(300, 145, 10, 50, BLACK);

  // Add branches to the cactus (right side)
  display.drawLine(305, 160, 315, 170, BLACK); // Right branch
  display.drawLine(315, 170, 305, 180, BLACK);
  display.fillRect(310, 165, 5, 5, BLACK); // Fill the right branch

  // Add branches to the cactus (left side)
  display.drawLine(300, 160, 310, 170, BLACK); // Left branch
  display.drawLine(310, 170, 300, 180, BLACK);
  display.fillRect(305, 165, 5, 5, BLACK); // Fill the left branch

  // Draw stick-figure human
  // Man's body
  display.fillRect(180, 110, 40, 60, BLACK);

  // Man's head
  display.fillCircle(200, 100, 15, GRAY);

  // Man's eyes
  display.drawCircle(zoomedEye.x, zoomedEye.y, zoomedEye.z, BLACK);
  display.drawCircle(205, 95, 2, BLACK);

  // Man's nose
  display.drawPixel(200, 100, BLACK);

  // Man's mouth
  display.drawLine(195, 105, 205, 135, BLACK);

  // Hat
  display.fillRect(190, 80, 20, 10, BLACK);
  display.fillRect(185, 70, 30, 10, BLACK);

  // Suit
  display.fillRect(185, 110, 30, 40, BLACK);

  // Tie
  display.drawLine(200, 125, 200, 175, GRAY);

  // Arms
  display.drawLine(175, 120, 185, 130, BLACK);
  display.drawLine(225, 120, 215, 130, BLACK);

  // Realistic Legs
  display.drawLine(200, 170, 185, 190, BLACK);
  display.drawLine(200, 170, 205, 190, BLACK);

  // Shoes
  display.fillRect(182, 190, 8, 5, BLACK);
  display.fillRect(205, 190, 8, 5, BLACK);

  // Draw stick-figure human
  // Man's body
  display.fillRect(100, 120, 20, 40, BLACK); // Move 10 pixels down

  // Man's head
  display.fillCircle(110, 110, 10, GRAY); // Move 10 pixels down

  // Man's eyes
  display.drawCircle(107, 107, 1, BLACK); // Move 10 pixels down
  display.drawCircle(113, 107, 1, BLACK); // Move 10 pixels down

  // Man's nose
  display.drawPixel(110, 110, BLACK); // Move 10 pixels down

  // Man's mouth
  display.drawLine(107, 113, 113, 113, BLACK); // Move 10 pixels down

  // Hat
  display.fillRect(105, 100, 15, 5, BLACK); // Move 10 pixels down

  // Arms
  display.drawLine(95, 130, 100, 115, BLACK);  // Move 10 pixels down
  display.drawLine(115, 130, 110, 115, BLACK); // Move 10 pixels down

  // Legs
  display.drawLine(105, 160, 100, 200, BLACK); // Move 10 pixels down
  display.drawLine(105, 160, 110, 200, BLACK); // Move 10 pixels down
}

//--------------------------------------------------
void setup() {
  Serial.begin((57600));                // start a serial port at 57600 BAUD
  delay(1000);                          // wait one second aka 1000ms
  Serial.println(" A Boolean Handaxe"); // send a message over serial

  setupDisplay();

  // time of flight
  Wire.begin(PIN_SDA, PIN_SCL);
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
  }

  delay(1000);
  // display.clearDisplay();
}

//--------------------------------------------------
void loop() {
  unsigned long now = millis(); // get the current time

  if (now > lastFrame + 1000 / 10) {
    lastFrame = now;

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    Serial.println(measure.RangeStatus);
    display.clearDisplayBuffer();

    if (measure.RangeStatus != 4) {
      // Serial.println(" in range ");
      distance = measure.RangeMilliMeter;
      if (distance < 100) {
        // echt dichtbij

        for (int i = 0; i < 10; ++i) {
          int x1 = 200;            // Center X
          int y1 = 120;            // Center Y
          int x2 = random(0, 400); // Random X on the screen
          int y2 = random(0, 240); // Random Y on the screen

          int thickness = random(1, 3); // Random thickness between 1 and 2

          for (int t = 0; t < thickness; ++t) {
            int zigzag = random(-5, 6); // Random zigzag offset
            display.drawLine(x1 + t + zigzag, y1 + t + zigzag, x2 + t + zigzag,
                             y2 + t + zigzag, BLACK);
          }
        }
      }

      else {
        // Adjust the circle size based on the distance
        int radius = map(measure.RangeMilliMeter, 0, 450, 240,
                         50); // Adjust the range as needed
        int x = 70;
        int y = 90;

        // Serial.print("Distance (mm): ");
        // Serial.println(measure.RangeMilliMeter);

        // Additional display content based on the distance
        // display.setTextSize(3);
        // display.setTextColor(BLACK);
        // display.setCursor(10, 10);
        // display.println(measure.RangeMilliMeter);

        // verrekijker
        display.fillRect(0, 0, 400, 240, BLACK);
        display.fillCircle(100, 120, radius, WHITE);
        display.fillCircle(300, 120, radius, WHITE);

        // Draw stick-figure human
        drawHuman();
      }
    } else {
      Serial.println(" out of range ");
      // verrekijker
      display.fillRect(0, 0, 400, 240, BLACK);
      display.fillCircle(100, 120, 90, WHITE);
      display.fillCircle(300, 120, 90, WHITE);
    }
    display.refresh();
  }
}
