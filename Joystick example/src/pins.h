#ifndef _PINS_PINS_H_
#define _PINS_PINS_H_


// PRINTER
#define PIN_PRINTER_RX 21 // printer data out GREEN
#define PIN_PRINTER_TX 20 // printer data in YELLOW

// I2C pins
#define PIN_SDA 1
#define PIN_SCL 2


// Colors
#define BLACK 0
#define WHITE 1
#define GRAY 2

// RGB LED on board
#define PIN_RGB_LED 38

// LCD display pins
#define PIN_LCD_CLK 12
#define PIN_LCD_DI 13
#define PIN_LCD_CS 14


// Joystick defines
// GPIO pins on the Joy Featherwing for reading button presses. These should not be changed.
#define BUTTON_RIGHT 6
#define BUTTON_DOWN 7
#define BUTTON_LEFT 9
#define BUTTON_UP 10
#define BUTTON_SEL 14
// GPIO Analog pins on the Joy Featherwing for reading the analog stick. These should not be changed.
#define STICK_H 3
#define STICK_V 2

#ifndef STICK_CENTER_POINT
#define STICK_CENTER_POINT 512 // Analog stick will read 0...1024 along each axis
#endif
#ifndef STICK_L_CORRECTION
#define STICK_L_CORRECTION -55
#endif
#ifndef STICK_R_CORRECTION
#define STICK_R_CORRECTION 50
#endif
#ifndef STICK_U_CORRECTION
#define STICK_U_CORRECTION 20
#endif
#ifndef STICK_D_CORRECTION
#define STICK_D_CORRECTION -20
#endif



#endif