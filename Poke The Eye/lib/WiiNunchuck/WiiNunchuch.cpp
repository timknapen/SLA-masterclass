#include "WiiNunchuck.h"

//------------------------------------------
void WiiNunchuck::begin(int _sda, int _scl) {
  sda = _sda;
  scl = _scl;

  //   Wire.begin();
  if (!Wire.begin(sda, scl, TWI_FREQ)) {
    Serial.println("Nunchuck failed to start up");
    while (true) {
      ;
    }
  }
  nunchuck_init();
  z_button = c_button = 0;
  joyX = joyY = 0;
}

//------------------------------------------
void WiiNunchuck::update() {
  send_zero();
  delayMicroseconds(275);
  Wire.requestFrom(0x52, 6);
  delayMicroseconds(100);

  int bufPos = 0;

  while (Wire.available() && bufPos < 6) {
    dataBuf[bufPos] = nunchuk_decode_byte(Wire.read());
    bufPos++;
  }

  // run through data if all == 255, this is bad data!
  // bool data255 = true;
  // for(int i = 0; i < 6; i++){
  //   if(dataBuf[i] != 255){
  //     data255 = false;
  //     break;
  //   }
  // }
  // if(data255){
  //   return;
  // }

  // DATA
  /*
  0 Joy X
  1 Joy Y
  2 Acc X ?
  3 Acc Y ?
  4 Acc Z ?
  5 c and z button
  */

  if (bufPos >= 5) {

    // joystick
    int joy_x_axis = dataBuf[0];
    int joy_y_axis = dataBuf[1];
    joyX = joy_x_axis - 124;
    joyY = joy_y_axis - 130;

    // accelerometer
    aX = dataBuf[2];
    aY = dataBuf[3];
    aZ = dataBuf[4];

    b = dataBuf[5];
    // button data
    z_button = 1;
    c_button = 1;

    if ((dataBuf[5] >> 0) & 1) {
      z_button = 0;
    }
    if ((dataBuf[5] >> 1) & 1) {
      c_button = 0;
    }
  }

}

//------------------------------------------
void WiiNunchuck::printRaw() {
  Serial.print("Wii: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(dataBuf[i]);
    Serial.print(", ");
  }
  uint8_t b = dataBuf[5];
  for (int i = 0; i < 8; i++) {
    Serial.print((b >> i) & 0x01);
  }
  Serial.println();
}

//------------------------------------------
void WiiNunchuck::nunchuck_init() {
  Wire.beginTransmission(0x52);
  Wire.write(0x40);
  Wire.write(0x00);
  Wire.endTransmission();
}

//------------------------------------------
void WiiNunchuck::send_zero() {
  Wire.beginTransmission(0x52);
  Wire.write(0x00);
  Wire.endTransmission();
}

//------------------------------------------
char WiiNunchuck::nunchuk_decode_byte(char x) {
  x = (x ^ 0x17) + 0x17;
  return x;
}