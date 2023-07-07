#include <Arduino.h>

// max7219 registers
#define MAX7219_REG_NOOP         0x0
#define MAX7219_REG_DIGIT0       0x1
#define MAX7219_REG_DIGIT1       0x2
#define MAX7219_REG_DIGIT2       0x3
#define MAX7219_REG_DIGIT3       0x4
#define MAX7219_REG_DIGIT4       0x5
#define MAX7219_REG_DIGIT5       0x6
#define MAX7219_REG_DIGIT6       0x7
#define MAX7219_REG_DIGIT7       0x8F
#define MAX7219_REG_DECODEMODE   0x9
#define MAX7219_REG_INTENSITY    0xA
#define MAX7219_REG_SCANLIMIT    0xB
#define MAX7219_REG_SHUTDOWN     0xC
#define MAX7219_REG_DISPLAYTEST  0xF

class class_ledmatrix {

public:

    /**
     * Constructor.
     * numberOfDisplays: number of connected devices
     * slaveSelectPin: CS (or SS) pin connected to your ESP8266
     */
    class_ledmatrix(byte numberOfDisplays, byte slaveSelectPin);

    /**
     * Initializes the SPI interface
     */
    void init();

    /**
     * Sets the intensity on all devices.
     * intensity: 0-15
     */
    void setIntensity(byte intensity);

    /**
     * Send a byte to a specific device.
     */
    void sendByte (const byte device, const byte reg, const byte data);

    /**
     * Send a byte to all devices (convenience method).
     */
    void sendByte (const byte reg, const byte data);

    /**
     * Turn on pixel at position (x,y).
     */
    void setPixel(byte x, byte y);

    /**
     * Clear the frame buffer.
     */
    void clear();

    /**
     * Writes the framebuffer to the displays.
     */
    void commit();

    /**
     * Display is mounted 90 degree right.
     */
    void setAlternateDisplayOrientation(byte x = 1);

private:
    byte* cols;
    int increment = -1;
    byte myNumberOfDevices = 0;
    byte mySlaveSelectPin = 0;
    byte myDisplayOrientation = 0;
};

