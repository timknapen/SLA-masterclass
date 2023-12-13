#include "TKPoint.h"

/**** POINT *******/
//------------------------------------------------------------
TKPoint::TKPoint() {
  x = 0;
  y = 0;
  z = 0;
}
//------------------------------------------------------------
TKPoint::TKPoint(float _x, float _y, float _z) {
  x = _x;
  y = _y;
  z = _z;
}
//------------------------------------------------------------
TKPoint::TKPoint(float _x, float _y) {
  x = _x;
  y = _y;
  z = 0;
}
//------------------------------------------------------------
void TKPoint::set(float _x, float _y, float _z) {
  x = _x;
  y = _y;
  z = _z;
}
//------------------------------------------------------------
void TKPoint::set(float _x, float _y) {
  x = _x;
  y = _y;
  z = 0;
}
//------------------------------------------------------------
void TKPoint::set(TKPoint p) {
  x = p.x;
  y = p.y;
  z = p.z;
}

//------------------------------------------------------------
TKPoint TKPoint::operator+(const TKPoint p) {
  return TKPoint(p.x + x, p.y + y, p.z + z);
}

//------------------------------------------------------------
TKPoint TKPoint::operator-(const TKPoint p) {
  return TKPoint(x - p.x, y - p.y, z - p.z);
}

//------------------------------------------------------------
TKPoint TKPoint::operator*(const float s) {
  return TKPoint(s * x, s * y, s * z);
}

//------------------------------------------------------------
void TKPoint::operator+=(const TKPoint p) {
  x += p.x;
  y += p.y;
  z += p.z;
}

//------------------------------------------------------------
void TKPoint::normalize() {
  float d = sqrt(x * x + y * y + z * z);
  if (d != 0) {
    x /= d;
    y /= d;
    z /= d;
  }
}

//------------------------------------------------------------
void TKPoint::rotate(float angle) {
  // rotate our point in the XY plane
  float angle_rad = PI * angle / 180.0f;
  float sin_a = sin(angle_rad);
  float cos_a = cos(angle_rad);

  float x1 = (int)(x * cos_a - y * sin_a);
  float y1 = (int)(y * cos_a + x * sin_a);

  x = x1;
  y = y1;
}

/****** OPERATIONS *************/
//------------------------------------------------------------
float dist(float x1, float y1, float x2, float y2) {
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

//------------------------------------------------------------
float dist(TKPoint p1, TKPoint p2) { return dist(p1.x, p1.y, p2.x, p2.y); }

//------------------------------------------------------------
float len(TKPoint p) { return dist(0, 0, p.x, p.y); }

//------------------------------------------------------------
void normalize(TKPoint &p) {
  float d = sqrt(p.x * p.x + p.y * p.y);
  if (d != 0) {
    p.x /= d;
    p.y /= d;
  }
}
