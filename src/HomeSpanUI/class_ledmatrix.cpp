#include <SPI.h>
#include "class_ledmatrix.h"

class_ledmatrix::class_ledmatrix(byte numberOfDevices, byte slaveSelectPin)
{
    myNumberOfDevices = numberOfDevices;
    mySlaveSelectPin = slaveSelectPin;
    cols = new byte[numberOfDevices * 8];
}

void class_ledmatrix::init()
{
    pinMode(mySlaveSelectPin, OUTPUT);

    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    for (byte device = 0; device < myNumberOfDevices; device++)
    {
        sendByte(device, MAX7219_REG_SCANLIMIT, 7);   // show all 8 digits
        sendByte(device, MAX7219_REG_DECODEMODE, 0);  // using an led matrix (not digits)
        sendByte(device, MAX7219_REG_DISPLAYTEST, 0); // no display test
        sendByte(device, MAX7219_REG_INTENSITY, 0);   // character intensity: range: 0 to 15
        sendByte(device, MAX7219_REG_SHUTDOWN, 1);    // not in shutdown mode (ie. start it up)
    }
}

void class_ledmatrix::sendByte(const byte device, const byte reg, const byte data)
{
    digitalWrite(mySlaveSelectPin, LOW);
    for (int i = 0; i < myNumberOfDevices; i++)
    {
        SPI.transfer((i == device) ? reg : (byte)0);
        SPI.transfer((i == device) ? data : (byte)0);
    }
    digitalWrite(mySlaveSelectPin, HIGH);
}

void class_ledmatrix::sendByte(const byte reg, const byte data)
{
    digitalWrite(mySlaveSelectPin, LOW);
    for (int i = 0; i < myNumberOfDevices; i++)
    {
        SPI.transfer(reg);
        SPI.transfer(data);
    }
    digitalWrite(mySlaveSelectPin, HIGH);
}

void class_ledmatrix::setIntensity(const byte intensity)
{
    sendByte(MAX7219_REG_INTENSITY, intensity);
}

void class_ledmatrix::clear()
{
    for (byte col = 0; col < myNumberOfDevices * 8; col++)
    {
        cols[col] = 0;
    }
}

void class_ledmatrix::commit()
{
    if (myDisplayOrientation)
    {
        for (byte dcol = 0; dcol < 8; dcol++)
        {
            digitalWrite(mySlaveSelectPin, LOW);
            for (int dev = 0; dev < myNumberOfDevices; dev++)
            {
                byte b = 0;
                for (byte fcol = 0; fcol < 8; fcol++)
                    if (cols[dev * 8 + fcol] & (1 << dcol))
                        b |= (128 >> fcol);
                SPI.transfer(dcol + 1);
                SPI.transfer(b);
            }
            digitalWrite(mySlaveSelectPin, HIGH);
        }
    }
    else
    {
        for (byte col = 0; col < 8; col++)
        {
            digitalWrite(mySlaveSelectPin, LOW);
            for (int dev = 0; dev < myNumberOfDevices; dev++)
            {
                SPI.transfer(col + 1);
                SPI.transfer(cols[dev * 8 + col]);
            }
            digitalWrite(mySlaveSelectPin, HIGH);
        }
    }
}

void class_ledmatrix::setAlternateDisplayOrientation(byte x)
{
    myDisplayOrientation = x;
}

void class_ledmatrix::setPixel(byte x, byte y)
{
    bitWrite(cols[x], y, true);
}
