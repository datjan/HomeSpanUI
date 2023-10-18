# Devices

HomeSpanUI supports several devices.

## Lights

### LED (dimmable)

Single LED, dimmable.

```
          __   
         |__|     
         |__|     
          ||     
          ||
  Kathode || 
           | Anode 
```         
| ESP32 |  LED |
| ------------- | ------------- |
| GND  | Kathode | 
| Pin  | Resistor -> Anode |


### LED RGB

Red,Gree,Blue LED.

```  
          ____   
         |____|     
         |____|     
          ||||     
          |||| Anode Blue
  Kathode ||| Anode Green 
           | Anode Red 
```
| ESP32 |  RGB LED |
| ------------- | ------------- |
| GND  | Kathode | 
| Pin  | Resistor -> Red Anode |
| Pin  | Resistor -> Green Anode |
| Pin  | Resistor -> Blue Anode |


### 8x8 Matrix - MAX7219 (up to three different combinations)

The matrix can show three symbols. The on/off led will be represented as bit code.

For example Pacman:
```
00111100
01001110
11001100
11111000
11111000
11111100
01111110
00111100
```
Convert the bits to hex:
```
3C
4E
CC
F8
F8
FC
7E
3C
```
Within HomeSpanUI you can use this to represent the light combination:
```
3C4ECCF8F8FC7E3C
```

Wiring:
```
    _______________
   |               |
   |    MAX7219    |
   |_______________|
   |   |   |   |   |
  VCC GND MOSI CS CLK
          DIN  SS SCK
```
| ESP32 |  Sensor MAX7219 |
| ------------- | ------------- |
| 3v3  | VCC | 
| GND  | GND |
| SPI MOSI  | MOSI/DIN |
| SPI CLK  | CLK/SCK |
| Pin  | CS/SS |



## Relais

### Outlet

Simple relais.

```
    _________
   |         |
   |  Relais |
   |_________|
   |    |    |   
  VCC  GND  IN   
```
| ESP32 |  Relais 3.3V/5V |
| ------------- | ------------- |
| 3.3V/5V  | VCC | 
| GND  | GND |
| GPIO  | IN |



## Sensors

### Contact

Simple contact sensor.

```
       -------------- 3v3  
      |               
   \        10kOhm   
    \       --X----- GND
     \     |          
      |     |          
       -------------- Pin  
```
| ESP32 |  Potential free contact |
| ------------- | ------------- |
| 3v3  | Contact pin 1 | 
| Pin  | Contact pin 2 |
| Pin  | 10kOhm -> GND |


### Light - BH1750

Light sensor.

```
    _______________
   |               |
   |    BH1750     |
   |_______________|
   |   |   |   |   |
  VCC GND SCL SDA ADR
```
| ESP32 |  Sensor BH1750 |
| ------------- | ------------- |
| 3v3  | VCC | 
| GND  | GND |
| Pin I2C SCL | SCL |
| Pin I2C SDA | SDA |


### Light - TEMT6000

Light sensor.

```
    _______________
   |               |
   |   TEMT6000    |
   |_______________|
   |      |       |  
  VCC    GND     OUT 
```
| ESP32 |  Sensor TEMT6000 |
| ------------- | ------------- |
| 3v3  | VCC | 
| GND  | GND |
| Pin ADC1 | OUT |


### Motion - HC-SR501

Motion sensor.

```
   _________ 
  |         |
  | HCSR501 |
  |_________|
  |    |    |
  GND DO  VCC
```
| ESP32 |  Sensor HCSR501 |
| ------------- | ------------- |
| 3v3  | VCC | 
| GND  | GND |
| Pin GPIO DI | DO |


### Motion - SW420

Motion sensor.

```
   ________    
  |        |   
  | SW420  | 
  |________|  
  |   |    |  
 DO  GND  VCC
```
| ESP32 |  Sensor SW420 |
| ------------- | ------------- |
| 3v3  | VCC | 
| GND  | GND |
| Pin GPIO DI | DO |


### Smoke - MQ-2

Smoke sensor.

```
   ________    
  |        |   
  | MQ-2   | 
  |________|  
  |   |    |  
 DO  GND  VCC
```
| ESP32 |  Sensor MQ-2 |
| ------------- | ------------- |
| 3v3  | VCC | 
| GND  | GND |
| Pin GPIO DI | DO |


### Temperature - DS18B20

Temperature sensor.

```
         ------- black (GND)
 ____   |     
(____|--|------- red (VCC)
        |    |
        |    X 4,7 kOhm
        |    |
         ------- yellow (DQ)
```
| ESP32 |  Sensor DS18B20 |
| ------------- | ------------- |
| 5V  | VCC (red) | 
| GND  | GND (black) |
| Pin ADC | DQ (yellow) |
| Pin ADC | 4,7 kOhm -> 5v |


### Temperature & Humidity - DHT11

Temperature and humidity sensor.

```
       ________
      |        |
      |  DHT   |
      |________|
      |  |  |  |
    VCC  D  N  GND
```
| ESP32 |  Sensor DHT11 |
| ------------- | ------------- |
| 3V3  | VCC | 
| 3V3  | 10kOhm -> D | 
| GND  | GND |
| Pin ADC | D |


### Temperature & Humidity - DHT22

Temperature and humidity sensor.

```
       ________
      |        |
      |  DHT   |
      |________|
      |  |  |  |
    VCC  D  N  GND
```
| ESP32 |  Sensor DHT22 |
| ------------- | ------------- |
| 3V3  | VCC | 
| 3V3  | 10kOhm -> D | 
| GND  | GND |
| Pin ADC | D |


## Other

### Button

Button with three states: single press, long press, double press

```
          | pin 2    
          |     
       \   
     |--\   
         \   
          |   
          | pin 1 
```
| ESP32 |  Potential free button |
| ------------- | ------------- |
| GND  | Button pin 2 | 
| Pin GPIO  | Button pin 1 | 
| Pin GPIO  | 10kOhm -> Board 3v3 | 


### Doorbell

Simple Doorbell.

```
          | pin 2    
          |     
       \   
     |--\   
         \   
          |   
          | pin 1 
```
| ESP32 |  Potential free button |
| ------------- | ------------- |
| GND  | Button pin 2 | 
| Pin GPIO  | Button pin 1 | 
| Pin GPIO  | 10kOhm -> Board 3v3 | 


### Security System - Armed Contact and Trigger Contact

This security system has two contacts. One for the trigger line and one for arm and disarm.
```
       -------------- 3v3  
      |               
   \        10kOhm   
    \       --X----- GND
     \     |          
      |     |          
       -------------- Pin 
```
| ESP32 |  Potential free contact |
| ------------- | ------------- |
| 3v3  | Contact pin 1 | 
| Pin GPIO  | Contact pin 2 | 
| Pin GPIO  | 10kOhm -> Board GND | 


### Security System - Contact for every state in HomeKit

This security system has four contact sensors for the states: away, triggered, night, stay
```
       -------------- 3v3  
      |               
   \        10kOhm   
    \       --X----- GND
     \     |          
      |     |          
       -------------- Pin 
```
| ESP32 |  Potential free contact |
| ------------- | ------------- |
| 3v3  | Contact pin 1 | 
| Pin GPIO  | Contact pin 2 | 
| Pin GPIO  | 10kOhm -> Board GND | 



---

[↩️](../README.md) Back to the Welcome page
