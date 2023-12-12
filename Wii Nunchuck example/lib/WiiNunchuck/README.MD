
# Nintendo Wii Nunchucks


## Wiring

    white   GND
    red     +3.3V
    green   SDA (data)
    yellow  SCL (clock)


## Power

The nunchuck runs at 3.3 volt.


## Protocol

The Wii nunchuck uses I2C at 400Kb/s.  
Set the I2C frequency like so:

    #define TWI_FREQ 400000L


For the MCU to communicate with the nunchuck, it must send a handshake.

First send 2 bytes:
    
    0x40, 0x00

Send one byte `0x00` each time you request data from the nunchuck.

The data from the nunchuck will come back in 6 byte chunks.
