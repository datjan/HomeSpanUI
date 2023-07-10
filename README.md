# HomeSpanUI :house:
HomeSpanUI is an easy-to-use WebUI running on your ESP32 controller. It is based on the famous [HomeSpan](https://github.com/HomeSpan/HomeSpan) library which makes it possible to create your own ESP32-based HomeKit devices.

You can install HomeSpanUI on nearly every ESP32 controller and access the UI with a simple web browser. A few clicks and you can add own devices and pair them with HomeKit.


### Easy? For sure! :thumbsup:
:one: Upload HomeSpanUI to your ESP32

:two: Configure your device on the web interface

:three: Wire the device hardware to the ESP32

:four: Pair the controller with HomeKit

![alt text](https://github.com/datjan/HomeSpanUI/blob/main/docs/images/device_example.png?raw=true)


# Setup Controller :computer:
### Hardware
The following hardware is required:
```
- ESP32 (WROOM, LOLIN Lite, LOLIN S3 etc.)
```
For advanced information read [Hardware documentation](docs/HardwareController.md)

### Development Environment
This sketch is for following development environment
```
Arduino IDE 2.0.0 or higher
```

Required "Boards Manager" for Arduino IDE
```
ESP32 by Espressif Systems  - https://github.com/espressif/arduino-esp32
from Boards Manager URL: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Required "Libraries" for Arduino IDE
```
HomeSpan - https://github.com/HomeSpan/HomeSpan
BH1750 support - https://github.com/claws/BH1750
OneWire support - https://github.com/PaulStoffregen/OneWire
DS18B20 support - https://github.com/milesburton/Arduino-Temperature-Control-Library
DHT support - https://github.com/adafruit/DHT-sensor-library
```

For advanced information read [Dependencies documentation](docs/Dependencies.md)

### Setup before Upload
Setup WiFi-Connection in "setup.h":
```
const char* wlan_ssid = "";     // Wifi SSID, the device will try to connect
const char* wlan_pwd = "";      // Wifi PASSWORD, the device will try to connect
```
For advanced information read [WiFi documentation](docs/WiFi.md)

### Access web interface
After the controller has connected to your WiFi you can access the web interface typing the IP-address into your web-browser.

### Device setup
Now you can setup the devices you want to use with your controller

### Pair with HomeKit
At least pair HomeSpanUI with your HomeKit with the help of the qr-code published on the webinterface.

For advanced information read [HomeKit pairing documentation](docs/HomeKitPairing.md)

# Features :star2:
### HomeKit Modes
HomeSpanUI supports following modes:
```
* DEVICE - one device
* BRIDGE - up to 8 devices
```

### Devices
The following devices/hardware will be supported:

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
* Temperature & Humidity - DHT11
* Temperature & Humidity - DHT22
```

Other
```
* Button
* Doorbell
* Security System - Armed Contact and Trigger Contact
* Security System - Contact for every state in HomeKit
```

### Easy controller setup
The web interface features an easy controller setup for the usage of multiple esp32 homekit controllers.

![alt text](https://github.com/datjan/HomeSpanUI/blob/main/docs/images/controller_settings_example.png?raw=true)

### Easy device setup and wiring
The devices can be added and configured in an easy way. The web interface shows the wiring of the hardware parts of your homekit device.

![alt text](https://github.com/datjan/HomeSpanUI/blob/main/docs/images/device_settings_example.png?raw=true)

### Device status
The web interface is fully dynamic and shows you the state of each device in nearly the same way the HomeKit app does.

### Action log
HomeSpanUI logs the last 20 activities on the controller.

# Notes :page_facing_up:
This project is just beginning, so there are still a few to-dos
```
* not every device has its icon
* maybe some refreshing problems may appear on the web interface
* some parts of the code could be more lightweight to save flash
* first response of web interface is to slow
```
Actual development on this project:
```
* Action Log supports MQTT send
* Outlet device
* Garage door device
* Leak device
* Device sources from MODBUS, MQTT and REST APIs
```

