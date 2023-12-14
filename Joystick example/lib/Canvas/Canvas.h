#ifndef _TK_CANVAS_H_
#define _TK_CANVAS_H_

#include "Adafruit_GFX.h"

class Canvas : public GFXcanvas1 {
private:
public:
  Canvas(uint16_t w, uint16_t h);
  ~Canvas(void);
  void drawFatLine(int x0, int y0,  // first point
                   int x1, int y1,  // second point
                   int strokeWidth, // stroke width
                   uint16_t color);

  void clear();
};

#endif