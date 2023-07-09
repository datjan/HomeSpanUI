# WiFi connection

HomeSpanUI needs to connect to your WiFi-Network. The WLAN access data is required for this.

There are two ways to publish the credentials to HomeSpanUI.

## In Source Code
Befor uploading the Adruino-sketch you can enter the WiFi-Credentials into the sketch itself.

Setup WiFi-Connection in "HomeSpanUI.ino":
```
const char* wlan_ssid = "";     // Wifi SSID, the device will try to connect
const char* wlan_pwd = "";      // Wifi PASSWORD, the device will try to connect
```

## Using Access Point
HomeSpan can publish an access point including a configuration webpage.
You can upload the Arduino-sketch without changes. After startup of the ESP32 you can use the "board_pin_ap" (default: GPIO 15) to force HomeSpan to launch the access point mode.

Shortly Pull Up this pin to activate the Access Point
```
       --- 3v3 
      |               
   \       
    \     
     \             
      |            
       --- Pin 15
```
| ESP32 |  Potential free contact |
| ------------- | ------------- |
| Board 3v3  | Contact pin 1 | 
| Board Pin 15  | Contact pin 2 |


A new WiFi-Network "HomeKitController" will appear and you can connect without a password.
The landingpage will open automatically. There you can configure your WiFi-Credentials.

---

[↩️](../README.md) Back to the Welcome page
