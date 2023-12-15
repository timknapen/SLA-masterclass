#include "Adafruit_VL53L0X.h"
#include "TKPoint.h"
#include "pins.h"
#include <Adafruit_GFX.h>
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
    Adafruit_SharpMem(&SPI, PIN_LCD_CS, width, height, 4000000);
// slower comm because we have noisy lines!
// this was the killer bug that fucked the presentation!

// setup the display
void setupDisplay() {
  SPI.begin(PIN_LCD_CLK, -1, PIN_LCD_DI, PIN_LCD_CS);
  display.begin();
  display.setRotation(2);
  display.clearDisplay();
}

float interpolateFloat(float start, float end, float pct) {
  // give me a start and end value and percentage (0-1)
  // I will give you the inbetween value
  // pct is a value between 0 and 1
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

  TKPoint eye1(195, 95, 2); // positie en radius van oog op zoom 0
  TKPoint eye2(10, 20, 50); // positie en radius van oog op zoom 100

  TKPoint body1(200, 170, 20);
  TKPoint leg1(185, 190, 5);

  TKPoint body2(110, 200, 200); // Adjust the position and radius as needed
  TKPoint leg2(105, 200, 100);  // Adjust the position and radius as needed

  TKPoint hoofd1(200, 100, 15); // Adjust the position and radius as needed
  TKPoint hoofd2(350, 180, 50); // Adjust the position and radius as needed

  // finds the zoom level within our scene
  float zoomLevel = findZoom(200, 100, distance);

  TKPoint zoomedEye = interpolate(eye1, eye2, zoomLevel);
  TKPoint zoomedBody = interpolate(body1, body2, zoomLevel);
  TKPoint zoomedLeg = interpolate(leg1, leg2, zoomLevel);
  TKPoint zoomedhoofd = interpolate(hoofd1, hoofd2, zoomLevel);

  // Draw sun
  display.fillCircle(zoomedBody.x, zoomedEye.y, zoomedhoofd.z * 2, 4);

  // Draw stick-figure human
  // Man's body
  display.fillRect(zoomedBody.x, zoomedBody.y, zoomedBody.z * 2, 50, 6);

  // Man's head
  display.fillCircle(zoomedhoofd.x, zoomedhoofd.y, zoomedhoofd.z, GRAY);

  // Man's eyes
  display.drawCircle(zoomedEye.x, zoomedEye.y, zoomedEye.z, BLACK);

  // Man's nose
  display.drawPixel(200, 100, BLACK);

  // Man's mouth
  display.drawLine(195, 105, 205, 135, BLACK);

  // Hat
  display.fillRect(zoomedEye.x, 80, 20, 10, 5);
  display.fillRect(185, zoomedBody.y, 30, 10, BLACK);

  // Suit
  display.fillRect(185, 110, 30, 40, BLACK);

  // Tie
  display.drawLine(200, 125, 200, 175, GRAY);

  // Arms
  display.drawLine(175, 120, 185, 130, BLACK);
  display.drawLine(225, 120, 215, 130, BLACK);

  // Realistic Legs
  display.drawLine(zoomedBody.x, zoomedBody.y, zoomedLeg.x, zoomedLeg.y, BLACK);

  display.drawLine(200, 170, 205, 190, BLACK);

  // Shoes
  display.fillRect(zoomedLeg.x - zoomedLeg.z / 2, zoomedLeg.y - zoomedLeg.z / 2,
                   zoomedLeg.z, zoomedLeg.z / 2, BLACK);

  display.fillRect(205, 190, 8, 5, BLACK);
}

//--------------------------------------------------
void setup() {
  Serial.begin(57600);                 // start a serial port at 57600 BAUD
  delay(1000);                         // wait one second aka 1000ms
  Serial.println(" The final zoomer"); // send a message over serial

  setupDisplay();

  display.clearDisplayBuffer();
  display.setTextSize(6);
  display.setTextColor(BLACK);
  display.setCursor(0, 20);
  display.println("  zoomer");
  display.refresh();
  delay(1000);

  // time of flight
  Wire.begin(PIN_SDA, PIN_SCL, TWI_FREQ);
  if (!lox.begin()) {
    display.println("Failed to boot VL53L0X");
    display.println("Failed to boot VL53L0X");

  } else {
    display.println("VL53L0X OK");
    display.println("VL53L0X OK");
  }
  display.refresh();

  delay(1000);
}

//--------------------------------------------------
void loop() {
  unsigned long now = millis(); // get the current time

  if (now > lastFrame + 1000 / 10) { // 10fps
    lastFrame = now;

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    Serial.println(measure.RangeMilliMeter);
    display.clearDisplayBuffer();

    if (measure.RangeStatus != 4) {
      // Serial.println(" in range ");
      distance = measure.RangeMilliMeter;
      if (distance < 100) {
        // echt dichtbij
        float zoomLevel2 = findZoom(100, 0, distance);

        for (int i = 0; i < 10; ++i) {
          TKPoint x3(430, 120, 20);
          TKPoint y3(200, 300, 200);
          TKPoint zoomedLine = interpolate(x3, y3, zoomLevel2);

          int x1 = 200;                              // Center X
          int y1 = 120;                              // Center Y
          int x2 = zoomedLine.x + random(-400, 100); // Random X on the screen
          int y2 = zoomedLine.y + random(-300, 100); // Random Y on the screen

          // Calculate thickness based on the zoom level
          int baseThickness =
              random(1, 7); // Random base thickness between 1 and 2
          int thickness =
              baseThickness +
              (int)(zoomLevel2 * 10); // Adjust the multiplier as needed

          for (int t = 0; t < thickness; ++t) {
            int zigzag = random(-5, 6); // Random zigzag offset
            display.drawLine(x1 + t + zigzag, y1 + t + zigzag, x2 + t + zigzag,
                             y2 + t + zigzag, BLACK);
          }
        }
      }

      else {
        // Adjust the circle size based on the distance
        int radius = map(measure.RangeMilliMeter, 0, 450, 240, 50);
        // Adjust the range as needed
        int x = 70;
        int y = 90;

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
