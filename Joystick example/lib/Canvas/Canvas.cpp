#include "Canvas.h"

Canvas::Canvas(uint16_t w, uint16_t h) : GFXcanvas1(w, h) {}

Canvas::~Canvas(void) {
  if (buffer)
    free(buffer);
}

void Canvas::drawFatLine(int x0, int y0,  // first point
                         int x1, int y1,  // second point
                         int strokeWidth, // stroke width
                         uint16_t color) {
  if (strokeWidth < 1) {
    return;
  }
  // create perpendicular vector
  float px = y1 - y0;
  float py = -(x1 - x0);
  // calculate length to normalize perpendicular vector
  float l = sqrt(px * px + py * py);
  if (l < 1) {
    // do not divide by zero
    // do not draw a line too short
    return;
  }
  // normalize and scale to strokewidth
  px = (float)strokeWidth * px / l;
  py = (float)strokeWidth * py / l;

  // finally draw our line!
  fillTriangle(x0 + (int)px, y0 + (int)py, // a
               x1 + (int)px, y1 + (int)py, // b
               x1 - (int)px, y1 - (int)py, // c
               color);
  fillTriangle(x0 + (int)px, y0 + (int)py, // a
               x1 - (int)px, y1 - (int)py, // c
               x0 - (int)px, y0 - (int)py, // d
               color);
}

void Canvas::clear(){
    fillScreen(1); // fill white
}