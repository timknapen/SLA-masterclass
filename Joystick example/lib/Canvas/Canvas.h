#ifndef _TK_CANVAS_H_
#define _TK_CANVAS_H_

#include "Adafruit_GFX.h"

class Canvas : public GFXcanvas1 {
private:
public:
  Canvas(uint16_t w, uint16_t h);
  ~Canvas(void);
  void drawFatLine(float x0, float y0,  // first point
                   float x1, float y1,  // second point
                   float strokeWidth, // stroke width
                   uint16_t color);
  void invert();
  void clear();
  int getWidth();
  int getHeight();
};

#endif