#ifndef POINT_H
#define POINT_H

#include "Arduino.h"

//------------------------------------------------------------
class TKPoint {
private:
public:
  TKPoint();
  TKPoint(float _x, float _y, float _z);
  TKPoint(float _x, float _y);
  void set(float _x, float _y, float _z);
  void set(float _x, float _y);
  void set(TKPoint p);
  float x;
  float y;
  float z;

  TKPoint operator+(const TKPoint p);
  TKPoint operator-(const TKPoint p);
  TKPoint operator*(float s);
  void rotate(float angle);
  void operator+=(const TKPoint p);

  void normalize();
};

float dist(float x1, float y1, float x2, float y2);
float dist(TKPoint p1, TKPoint p2);
float len(TKPoint p);
void normalize(TKPoint &p);

#endif
