#ifndef _WII_NUNCHUCH_H_
#define _WII_NUNCHUCH_H_

#define TWI_FREQ 10000L //400000L
#include "Arduino.h"
#include <Wire.h>

class WiiNunchuck {
public:
  void begin(int _sda, int _scl);
  void update();
  void printRaw(); // print raw data to serial
  int z_button;
  int c_button;
  int joyX, joyY;

  uint8_t b; // the last byte
  // accelerometer
  int aX, aY, aZ;

private:
  // I2C pins
  int sda;
  int scl;
  uint8_t dataBuf[6];

  char nunchuk_decode_byte(char x);
  void nunchuck_init();
  void send_zero();
};

#endif