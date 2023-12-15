#include "Canvas.h"

Canvas::Canvas(uint16_t w, uint16_t h) : GFXcanvas1(w, h) {}

Canvas::~Canvas(void) {
  if (buffer)
    free(buffer);
}

void Canvas::invert() {
  // invert our image
  uint32_t bytes = ((_width + 7) / 8) * _height;
  uint8_t *buf = getBuffer();
  for (int i = 0; i < bytes; i++) {
    buf[i] = ~buf[i];
  }
}

unsigned char reverse(unsigned char b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

void Canvas::invertAndFlip() {
  // invert our image
  uint32_t bytes = ((_width + 7) / 8) * _height;
  uint8_t *buf = getBuffer();
  uint8_t *tmpBuf = new uint8_t[bytes];
  for (int i = 0; i < bytes; i++) {

    tmpBuf[bytes - 1 - i] = ~buf[i];
  }
  for (int i = 0; i < bytes; i++) {
    buf[i] = reverse(tmpBuf[i]);
  }
  delete tmpBuf;
}

void Canvas::drawFatLine(float x0, float y0, // first point
                         float x1, float y1, // second point
                         float strokeWidth,  // stroke width
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
  fillTriangle(x0 + px, y0 + py, // a
               x1 + px, y1 + py, // b
               x1 - px, y1 - py, // c
               color);
  fillTriangle(x0 + px, y0 + py, // a
               x1 - px, y1 - py, // c
               x0 - px, y0 - py, // d
               color);
}

void Canvas::clear() {
  fillScreen(1); // fill white
}

int Canvas::getWidth() { return _width; }

int Canvas::getHeight() { return _height; }