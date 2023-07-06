# HomeSpanUI :house:
HomeSpanUI is an easy-to-use WebUI running on your ESP32 controller. It is based on the famous [HomeSpan](https://github.com/HomeSpan/HomeSpan) library which make it possible to create your own ESP32-based HomeKit devices.

You can install HomeSpanUI on nearly every ESP32 controller and access the UI with a simple web browser. A few clicks and you can add own devices and pair them with HomeKit.


### Easy? For sure! :thumbsup:
:one: Upload HomeSpanUI to your ESP32

:two: Configure your device on the web interface

:three: Pair the controller with HomeKit

![alt text](https://github.com/datjan/HomeSpanUI/blob/main/pictures/device_example.png?raw=true)


# Setup your Controller :computer:
### Hardware
The following hardware is required:
```
- ESP32 (WROOM, LOLIN Lite, LOLIN S3 etc.)
```
I recommend a "LOLIN32 Lite", because of size, energy management, quality and price.

### Development Environment
This sketch is for following development environment
```
Arduino IDE 2.1.0
```

 Required Board Manager
```
Arduino IDE 2.1.0 or higher - https://github.com/espressif/arduino-esp32
```

Following libraries are required
```
HomeSpan - https://github.com/HomeSpan/HomeSpan
BH1750 support - https://github.com/claws/BH1750
OneWire support - https://github.com/PaulStoffregen/OneWire
DS18B20 support - https://github.com/milesburton/Arduino-Temperature-Control-Library
DHT support - https://github.com/adafruit/DHT-sensor-library
```

### Setup before Upload
Setup WiFi-Connection in "HomeSpanUI.ino":
```
const char* wlan_ssid = "";     // Wifi SSID, the device will try to connect
const char* wlan_pwd = "";      // Wifi PASSWORD, the device will try to connect
```

# Features :star2:
### HomeKit Modes
The HomeSpanUI supports following modes:
```
* DEVICE - one device
* BRIDGE - up to 8 devices
```

### Devices
Lights
```
* LED (dimmable)
* LED RGB
* 8x8 Matrix - MAX7219 (up to three different combinations)
```

Sensors
```
* Contact
* Light - BH1750
* Light - TEMT6000
* Motion - HC-SR501
* Motion - SW420
* Smoke - MQ-2
* Temperature - DS18B20
* Temperature - DHT11
* Temperature - DHT22
```

Other
```
* Button
* Doorbell
* Security System - Armed Contact and Trigger Contact
* Security System - Contact for every state in HomeKit
```

### Responsive
The HomeSpanUI is fully responsive and shows you the state of each device on the web interface in nearly the same way the HomeKit app does.

# Notes :page_facing_up:
This project is just beginning, so there are still a few to-dos
```
* not every device has its icon
* maybe some refreshing problems may appear on the web interface
* some parts of the code could be more lightweight to save flash
* first response of web interface is to slow
```
