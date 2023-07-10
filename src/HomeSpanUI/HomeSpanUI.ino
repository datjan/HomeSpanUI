/*********************************************************************************
  HomeSpanUI
  www.homekitblogger.de
  HAVE FUN!
 ********************************************************************************/

// Configuration
#include "setup.h"
// HomeSpanUI
#include "homespanui.h"         // HomeSpanUI helper
// Devices Support - arduino libraries
#include <Wire.h>               // I2C communication library  for devices: BH1750, AHT10  https://www.arduino.cc/reference/en/language/functions/communication/wire/
#include <SPI.h>                // SPI communication library. for devices: MAX7219        https://www.arduino.cc/reference/en/language/functions/communication/spi/
// Devices Support - external libraries
#include <BH1750.h>             // BH1750 library   for devices: BH1750                   https://github.com/claws/BH1750
#include <OneWire.h>            // OneWire library  for devices: DS18B20                  https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h>  // Dallas library   for devices: DS18B20                  https://github.com/milesburton/Arduino-Temperature-Control-Library
#include "DHT.h"                // DHT library      for devices: DHT11, DHT22             https://github.com/adafruit/DHT-sensor-library
// Devices Support - internal libraries
#include "class_ledmatrix.h"    // LEDMATR library  for devices: MAX7219                  inspired from https://github.com/JohnChung93/esp32_youtube_max7219
// HomeSpan Support - external library
#include "HomeSpan.h"           // HomeSpan                                               https://github.com/HomeSpan/HomeSpan
#include "extras/PwmPin.h"      // HomeSpan library of various PWM functions
#include "homespan_devices.h"   // HomeKit Devices Defitions
// Webserver Support - part of esp32 board manager                                        https://github.com/espressif/arduino-esp32
#include "WebServer.h"          // WebServer library                                      
#include "webserver_index.h"    // Website index page
#include "webserver_style.h"    // Website css
#include "webserver_java.h"     // Website java
#include "webserver_javahelper.h"// Website java
#include "webserver_javaqr.h"  // Website java


WebServer webServer(80);  // create WebServer on port 80

// handle for NVS storage
nvs_handle controllerNVS;
nvs_handle deviceNVS;

uint8_t homekit_maxdevices_device = 1;         // In device mode only one single device can be configured
uint8_t homekit_maxdevices_bridge = aDEVICES;  // In bridge mode up to max devices can be configured

void setup() {

  Serial.begin(115200);

  logEntry(controllerData.homekit_name,(char*)controllerData.homekit_serialnumber, "start");

  // Get setup from on volatile storage
  readFromNVS();

  // Board Setup
  strcpy(controllerData.board_name, ARDUINO_VARIANT);  // Board names
  controllerData.board_psram_found = psramFound(); // PSRAM found?
  if (controllerData.board_psram_found) psramInit();  // PSRAM init
  pinMode(controllerData.board_pin_ap, INPUT_PULLDOWN);  // Default state LOW for Access Point pin
  Wire.begin(controllerData.board_pin_i2c_sda, controllerData.board_pin_i2c_scl); // I2C Init

  // If no suffix exists
  generateMdnsHostNameSuffix();

  // Setup HomeSpan
  homeSpan.setLogLevel(1);

  // Access Point (important to be at the beginning of homespan settings, maybe bug)
  homeSpan.setApSSID(ap_ssid);     // Access Point SSID
  homeSpan.setApPassword(ap_pwd);  // Access Point Password
  homeSpan.setApTimeout(300);      // Access Point TimeOut, how long stays alive once activated

  homeSpan.setSketchVersion(controllerData.version);

  homeSpan.setQRID(controllerData.homekit_setupid);                // 4-character Setup ID, default: HSPN
  homeSpan.setHostNameSuffix(controllerData.homekit_hostnamesuffix);                    // suffix for mDNS hostname (not used = HomeSpan will generate one)
  homeSpan.setPortNum(controllerData.homekit_port);  // change port number for HomeSpan so we can use port 80 for the Web Server
  homeSpan.enableOTA(ota_pwd);                       // enable OTA updates with password
  homeSpan.setMaxConnections(5);                     // reduce max connection to 5 (default is 8) since WebServer and a connecting client will need 2, and OTA needs 1
  homeSpan.setWifiCallback(setupWeb);                // need to start Web Server after WiFi is established

  homeSpan.setWifiCredentials(wlan_ssid, wlan_pwd);              // Wifi Access Data
  homeSpan.setPairingCode(controllerData.homekit_code);  // HomeKit Pairing Code

  homeSpan.setStatusCallback(statusUpdate);  // set callback function

  // PreInit Devices
  for (int i = 0; i < aDEVICES; i++) {
    if(deviceData[i].active == true && strcmp(deviceData[i].type.code, "maxled") == 0) {
      ledMatrix = new class_ledmatrix(1, deviceData[i].pin_1);
      ledMatrix->init();
      ledMatrix->setIntensity(0x03);
      ledMatrix->setAlternateDisplayOrientation(); // devices 90° right - comment out, if not needed
      ledMatrix->clear();
      ledMatrix->commit();
    }
  }

  // Create HomeKit Bridge
  if (strcmp(controllerData.homekit_type, "bridge") == 0) {
    // Set Category
    homeSpan.begin(Category::Bridges, controllerData.homekit_name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
    // Set BridgeAccessory within Category
    new SpanAccessory(1);
      new deviceAccessoryInformation(controllerData.homekit_name, controllerData.homekit_manufacturer, controllerData.homekit_serialnumber, controllerData.board_name, controllerData.version, 3);
      new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");

    // Set DeviceAccessory within BridgeAccessory within Category (for every Device)
    for (int i = 0; i < aDEVICES; i++) {
      if (deviceData[i].active == true) {
        new SpanAccessory(i + 2);
          new deviceAccessoryInformation(deviceData[i].name, controllerData.homekit_manufacturer, deviceData[i].homekitid, controllerData.board_name, controllerData.version, 0);
        // Set Service within DeviceAccessory within BridgeAccessory within Category
        for (int a = 0; a < deviceData[i].type.accessory_count; a++) { // Multi Services in one Accessory
          if (strcmp(deviceData[i].type.code, "led") == 0) new deviceGenericLED(i);
          if (strcmp(deviceData[i].type.code, "rgbled") == 0) new deviceRgbLED(i);
          if (strcmp(deviceData[i].type.code, "maxled") == 0) new deviceMax7219LED(i,a);
          if (strcmp(deviceData[i].type.code, "button") == 0) new deviceProgButton(i,i);
          if (strcmp(deviceData[i].type.code, "doorbell") == 0) new deviceDoorBell(i,i);
          if (strcmp(deviceData[i].type.code, "contact") == 0) new deviceContactSensor(i);
          if (strcmp(deviceData[i].type.code, "leak") == 0) new deviceLeakSensor(i);
          if (strcmp(deviceData[i].type.code, "security") == 0) new deviceSecuritySystem(i);
          if (strcmp(deviceData[i].type.code, "terxon") == 0) new deviceSecuritySystem(i);
          if (strcmp(deviceData[i].type.code, "garage") == 0) new deviceGarageDoorOpener(i);
          if (strcmp(deviceData[i].type.code, "ds18b20") == 0) new deviceTemperatureSensor(i);
          if (strcmp(deviceData[i].type.code, "dht11") == 0) new deviceTemperatureSensor(i);
          if (strcmp(deviceData[i].type.code, "dht22") == 0) new deviceTemperatureSensor(i);
          if (strcmp(deviceData[i].type.code, "bh1750") == 0) new deviceLightSensor(i);
          if (strcmp(deviceData[i].type.code, "temt6000") == 0) new deviceLightSensor(i);
          if (strcmp(deviceData[i].type.code, "sw420") == 0) new deviceMotionSensor(i);
          if (strcmp(deviceData[i].type.code, "hcsr501") == 0) new deviceMotionSensor(i);
          if (strcmp(deviceData[i].type.code, "mq2") == 0) new deviceSmokeSensor(i);
          if (strcmp(deviceData[i].type.code, "outlet") == 0) new deviceOutlet(i);
          if (strcmp(deviceData[i].type.code, "batmodbus") == 0) new deviceBatteryService(i);
        }
        // Set device restart required false
        deviceData[i].restartrequired = false;
      }
    }
  }

  // Create HomeKit Device
  if (strcmp(controllerData.homekit_type, "device") == 0) {
    if (deviceData[0].active == true) {
      // Set Category
      if (strcmp(deviceData[0].type.code, "contact") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "leak") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "button") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "doorbell") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "security") == 0) homeSpan.begin(Category::SecuritySystems, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "terxon") == 0) homeSpan.begin(Category::SecuritySystems, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "led") == 0) homeSpan.begin(Category::Lighting, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "rgbled") == 0) homeSpan.begin(Category::Lighting, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "maxled") == 0) homeSpan.begin(Category::Lighting, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "garage") == 0) homeSpan.begin(Category::GarageDoorOpeners, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "ds18b20") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "dht11") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "dht22") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "bh1750") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "temt6000") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "sw420") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "hcsr501") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "mq2") == 0) homeSpan.begin(Category::Sensors, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "outlet") == 0) homeSpan.begin(Category::Outlets, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      else if (strcmp(deviceData[0].type.code, "batmodbus") == 0) homeSpan.begin(Category::Other, deviceData[0].name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);
      // Set Accessory within Category
      new SpanAccessory();
        new deviceAccessoryInformation(deviceData[0].name, controllerData.homekit_manufacturer, deviceData[0].homekitid, controllerData.board_name, controllerData.version, 0);
        new Service::HAPProtocolInformation();
        new Characteristic::Version("1.1.0");
      // Set Service within Accessory within Category
      for (int a = 0; a < deviceData[0].type.accessory_count; a++) { // Multi Services in one Accessory
        if (strcmp(deviceData[0].type.code, "contact") == 0) new deviceContactSensor(0);
        else if (strcmp(deviceData[0].type.code, "leak") == 0) new deviceLeakSensor(0);
        else if (strcmp(deviceData[0].type.code, "button") == 0) new deviceProgButton(0,0);
        else if (strcmp(deviceData[0].type.code, "doorbell") == 0) new deviceDoorBell(0,0);
        else if (strcmp(deviceData[0].type.code, "security") == 0) new deviceSecuritySystem(0);
        else if (strcmp(deviceData[0].type.code, "terxon") == 0) new deviceSecuritySystem(0);
        else if (strcmp(deviceData[0].type.code, "led") == 0) new deviceGenericLED(0);
        else if (strcmp(deviceData[0].type.code, "rgbled") == 0) new deviceRgbLED(0);
        else if (strcmp(deviceData[0].type.code, "maxled") == 0) new deviceMax7219LED(0,a);
        else if (strcmp(deviceData[0].type.code, "garage") == 0) new deviceGarageDoorOpener(0);
        else if (strcmp(deviceData[0].type.code, "ds18b20") == 0) new deviceTemperatureSensor(0);
        else if (strcmp(deviceData[0].type.code, "dht11") == 0) new deviceTemperatureSensor(0);
        else if (strcmp(deviceData[0].type.code, "dht22") == 0) new deviceTemperatureSensor(0);
        else if (strcmp(deviceData[0].type.code, "bh1750") == 0) new deviceLightSensor(0);
        else if (strcmp(deviceData[0].type.code, "temt6000") == 0) new deviceLightSensor(0);
        else if (strcmp(deviceData[0].type.code, "sw420") == 0) new deviceMotionSensor(0);
        else if (strcmp(deviceData[0].type.code, "hcsr501") == 0) new deviceMotionSensor(0);
        else if (strcmp(deviceData[0].type.code, "mq2") == 0) new deviceSmokeSensor(0);
        else if (strcmp(deviceData[0].type.code, "outlet") == 0) new deviceOutlet(0);
        else if (strcmp(deviceData[0].type.code, "batmodbus") == 0) new deviceBatteryService(0);
      }
    } else {
      homeSpan.begin(Category::Sensors, controllerData.homekit_name,controllerData.homekit_hostnamebase,controllerData.homekit_modelname);  // Even if no accessory is configured we need to begin homespan otherwise the application will stop
    }
    // Set device restart required false
    deviceData[0].restartrequired = false;
  }

  char category[3]="";
  if (strcmp(controllerData.homekit_type, "bridge") == 0) sprintf(category,"%d",(int)Category::Bridges);
  else if (strcmp(controllerData.homekit_type, "device") == 0) {
      if (strcmp(deviceData[0].type.code, "contact") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "leak") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "button") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "doorbell") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "security") == 0) sprintf(category,"%d",(int)Category::SecuritySystems);
      else if (strcmp(deviceData[0].type.code, "terxon") == 0) sprintf(category,"%d",(int)Category::SecuritySystems);
      else if (strcmp(deviceData[0].type.code, "led") == 0) sprintf(category,"%d",(int)Category::Lighting);
      else if (strcmp(deviceData[0].type.code, "rgbled") == 0) sprintf(category,"%d",(int)Category::Lighting);
      else if (strcmp(deviceData[0].type.code, "maxled") == 0) sprintf(category,"%d",(int)Category::Lighting);
      else if (strcmp(deviceData[0].type.code, "garage") == 0) sprintf(category,"%d",(int)Category::GarageDoorOpeners);
      else if (strcmp(deviceData[0].type.code, "ds18b20") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "dht11") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "dht22") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "bh1750") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "temt6000") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "sw420") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "hcsr501") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "mq2") == 0) sprintf(category,"%d",(int)Category::Sensors);
      else if (strcmp(deviceData[0].type.code, "outlet") == 0) sprintf(category,"%d",(int)Category::Outlets);
      else if (strcmp(deviceData[0].type.code, "batmodbus") == 0) sprintf(category,"%d",(int)Category::Other);
  }
  getQrCodeText(atoi(controllerData.homekit_code),controllerData.homekit_setupid,atoi(category));
}  // end of setup()

//////////////////////////////////////

void loop() {
  // Process HomeSpan (HomeKit)
  homeSpan.poll();
  // Process Webserver
  webServer.handleClient();
  // Check AccessPoint pin, if closed
  if (digitalRead(controllerData.board_pin_ap) == HIGH) { startAccessPoint(); }  // Closed
}  // end of loop()

//////////////////////////////////////

void setupWeb() {
  Serial.print("Starting ESP32 Server Bridge...\n\n");
  webServer.begin();

  // Create web routines inline

  // Main Webpage ----------------------------------
  webServer.on("/", []() {
    webServer.send(200, "text/html", index_page);
  });
  webServer.on("/style.css", []() {
    webServer.send(200, "text/css", index_style);
  });
  webServer.on("/java.js", []() {
    webServer.send(200, "application/javascript", index_java);
  });
  webServer.on("/java_helper.js", []() {
    webServer.send(200, "application/javascript", index_javahelper);
  });
  webServer.on("/java_qr.js", []() {
    webServer.send(200, "application/javascript", index_javaqr);
  });

  // Rest Pictures ----------------------------------
  webServer.on("/pictures", []() {
    // Pic Encoding https://base64.guru/converter/encode/file or https://png-pixel.com
    String JSON = "[";
    JSON += "{\"id\":0,\"name\":\"none\",\"data\":\"image/png;base64\",\"base64\":\"iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII=\"},";
    JSON += "{\"id\":1,\"name\":\"light\",\"data\":\"image/png;base64\",\"base64\":\"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAARpSURBVFhH7ZffaxxVFMfPnZn9mf2RTbQh1UYKPoQiGiMitUZYg/Z/sIhILX3oc7WF4hJIoRQV8qCiFkSxfRHUByUUseibghELglZEVLSm2bTuzu7szs6Pe7wze5psnJ29M7s+aPEDlz3nMnfmO+d7f8zC/9zqMPodGuPc5G4EWATA+zjHKZzlCgJWGbDLbkf5dOLxP3+lS4diaIGtN0sHXMZOCXFPiJso1A181qVISBZKGeAlDmy5tFD/jLpjEVsgvj6dbaqdFSHssBgcGN8rsBfk+E7HTR2bKleb1BWJWAL52/lJw9ZWxbAHqStAmEAfxG8UxT2YP2BsUI+ULWtk4Ft3pZuW9vEgcVIYm+NcXf3j4tQY9UiJLLDh6GcZYw9ROjyMzWcz7ZcpkxLJ4uYbxTmuqGviYukLDbSY8BePzfcXH2t8SV2hRKogZ+rJKOKiwsQehAl2ktKBSCtYO18sKS11Xdw0SV0DiVJBD7ELOAp3dxcebVapqy/Sqghx5aji4iAqo3FQxAY/GKlAxnCOwh24lgpuR/Vj5AxsY/sdnKspQKdrDq9rfusLg/spCkUqUFgxQ+EWnrj2Rg7a1Rw4piZ+x8C8ngWrngb7pwx0vs2B+XUB3BsJMNcKfusnUkzFPRSGEmHis4C9bjsBiZwFiXzHF+odaJnbDbAaKbB/zkB6vuG/mflVAbQZE7Q9JjibwVmCiGkKQ5ELRAzs+lrW9gWkiiakJ1qQ2dUELWP7IlPzOqi3WZB+QIfkPgOSd7e6Y6Y6/m8vCOwahaHILWbKGoVbKAkXuNMd6lWSie8XDzXlgDru+DHTEBJ3mn7MmxoouT6rGyFw778jFcjcjjh7wepm26hJVyySkMnfA9eFuEJXdC/eNpPg9keUhiIVWDja3BQ7/3uUbuHZ7LQSlIXjrCf72ssQP8yVW+uUhiKfg11Oi7ajDL02D6KfveKFuWspy5QOJJLAwpEb34kPz1cp3UJms29vMWivqN65icXaZUoHErWCkAXllJg4v1DqI7PZudbHXsTfucuep0xKZIHKs9cbqPCnxeTm1CW1mTcC9nIx/plSuV6jXEpkgR6Fw7XPkfOzlPqE2dzPXnRxZXxB/4TSSMQS6FHA2gviUV9QGmpzn9W7VpzUT1AcmdgC2VGwXXSfEnPJ8PIwm3tXrzjS2qLnELsnuJ/KiC3QY/yI/qMYWaE0YHPAXoTl4iONK5TFQvrBGgZfgdRq6Tn/LENX8T+5vGresbcI3FD9o46luuvpXvW1rPLwb6KK8RlaoMcr5y90D+EeZmf2UrTN4sL+oZ8zksDKiy8FBO6anqZom2OHnhz6OUPNwZuk0ulAy2fHAm0U/vUWj1RBvVYLtKubG4E2CiMJNNvtQNNbRqCNwq1tsTgiPhAK37ct64eOaYLXNjarfqvr+vfigncZ8gt09VCMVMGbVM6cmUE1cVH8wZ+lrisO8oOnjx/f8Xk2DP+IQI9KpZLk+XxZ4UKmoV9aWlqKfe7+BwH4Cwqj8WU3BUcSAAAAAElFTkSuQmCC\"},";
    JSON += "{\"id\":2,\"name\":\"button\",\"data\":\"image/png;base64\",\"base64\":\"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\"},";
    JSON += "{\"id\":3,\"name\":\"switch\",\"data\":\"image/png;base64\",\"base64\":\"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAALnSURBVFhH7ZbPTxNREMdndktLKVIJ8uMCUbAYFaNS4klD/ANMPBiMwXgQE1APetMY0aAe9MjBHyR64KCJ6MEYzyIx8UQxKjGhiJF6AZWkBaGldHecfX1tKbSFhKfxsJ/Ddr6z293vznv73oCNjY2Njc2GQPmbF38fFWk0sRc0qjdMdOtkrPmfQhiok65RFEz8amLDh0AnLslTOcn7MH/faB2Q6xIStQOiV6bVQhQhxMeA2p1AZ0NIZrPIadB/P9iBiL0cepKZv868acLFkXO+h1KnWWWw+V6wW9PwhpRKqSrRocqjQSxBEJo1IG6QPJOEALsDXdtvSSnIMsiVO8aVeyalMg7WuuBkkwe2eh0yA7DI5l5/i0H/p3kIx0yZ5UEnagucbXwudcbggd5gmemCcU5VyZQSuppL4WhjiVSrmYmacOVNGCYjCaG5ptMuR8L37szOOUtrIsuYLu20anNHfO6C5iwq3Br0HPKC25GsFR+r4wm9QwgmbZC9H5eBEor5gaf2rO8bqynVxctkwLQXYXDXwKiTx75FZBSxv9oJm5zL3n8NWuuKZcQQNbdeHxQTVtyhbKaokj+OzAxWQJ1Xl9H6yLoe0Tm7pUZMN2Ewah3+U4TBpYqlnzzEyc9IEaGIIaP1seL6xbJfUz+sQBj83NYU5yEetmJVvJ+Ow1w8vb6tyVAoJiOxGI4M9RwWBVs2i+mpDJRg7Rb9H+elKszUbwNejmcmGiIMyDBj0Okof8QmRVlV8epLFF4EF6TKjbVQX3sbES9kwdWbdjoM9pIk11Znuc/KbxRrq2vf7YFtmzMLhWVocHLlVsf2CHJvdSn8D8auImg3pVRKZYkG1R69QLNgcrOwI3+zkEK0WwC9PBn+TbtFNM9WL3Dl0kObIu9QttwdryUdLvMFJ1iWJ7OKIQpzw/oEDbg9fN73XWazyGswResgORbGJvbxzeoNVNTyE7f8aLX8YW75Wwq2/DY2NjY2NhsB4A9uiwdJmrzcmQAAAABJRU5ErkJggg==\"},";
    JSON += "{\"id\":4,\"name\":\"contact\",\"data\":\"image/png;base64\",\"base64\":\"iVBORw0KGgoAAAANSUhEUgAAACgAAAAoCAYAAACM/rhtAAAAAXNSR0IArs4c6QAAAJZlWElmTU0AKgAAAAgABQESAAMAAAABAAEAAAEaAAUAAAABAAAASgEbAAUAAAABAAAAUgExAAIAAAARAAAAWodpAAQAAAABAAAAbAAAAAAAAAAMAAAAAQAAAAwAAAABd3d3Lmlua3NjYXBlLm9yZwAAAAOgAQADAAAAAQABAACgAgAEAAAAAQAAACigAwAEAAAAAQAAACgAAAAALFsVUgAAAAlwSFlzAAAB2AAAAdgB+lymcgAAAi1pVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IlhNUCBDb3JlIDYuMC4wIj4KICAgPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4KICAgICAgPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIKICAgICAgICAgICAgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIgogICAgICAgICAgICB4bWxuczp0aWZmPSJodHRwOi8vbnMuYWRvYmUuY29tL3RpZmYvMS4wLyI+CiAgICAgICAgIDx4bXA6Q3JlYXRvclRvb2w+d3d3Lmlua3NjYXBlLm9yZzwveG1wOkNyZWF0b3JUb29sPgogICAgICAgICA8dGlmZjpZUmVzb2x1dGlvbj4xMjwvdGlmZjpZUmVzb2x1dGlvbj4KICAgICAgICAgPHRpZmY6T3JpZW50YXRpb24+MTwvdGlmZjpPcmllbnRhdGlvbj4KICAgICAgICAgPHRpZmY6WFJlc29sdXRpb24+MTI8L3RpZmY6WFJlc29sdXRpb24+CiAgICAgIDwvcmRmOkRlc2NyaXB0aW9uPgogICA8L3JkZjpSREY+CjwveDp4bXBtZXRhPgqAvNGrAAAJ+klEQVRYCd1YeVBV5xX/3fvu21hFcWFREEWj8FhCRiWAPpc21lSNZlBn2j+6JG3/6Yz9p01mbEu1E9vaxbQzrUkbR6NGK8Zaa2yTmAQUl5mUQRAUKSouIMgqCLzt3tvfdy8Pnmgjj85kOj3M977tbPec853vfAD/pyDxu2Q2qaSkRNZ13XKYTcz/QzNwufe5gFBivPDf0I5JpiEgLW9FLDILDpGiDc7p77boet6bJ05s4ryO7TpbI9s1tn+x3cLcRX+evfBLMRwLCEvJsJBRXGxBaakKzFw89ws55d/+wffhjIvHM9mz0dnrQ11NHSyyBFXX4YAOryRDDvjw+9e2o+FU9RLgxukRHqa2T/pVnoTw+P0bUJUFiM9Y6Ht2kqRMt0C7YbNK7fNzJZ0EBXH8BHJu4qdU3NMCmtVho3KPZ/WEVRG8YYMrij68H0CS7JNnqQOy9s5bylzLoGWi5pcn6345zd8ve/a9Ic9kn2jxy433/ZjnCFuMQTAuC3oekNamwK9qjCgJ1umpdCjg13RYRNBwzZY6Gzpd7BU4dgUejyEv7J9xKcjIl+ZESKjv8/gnKDbMK1qO2rY+tFAJoegFWcV8rtW196Oxz+ef45CUhjAPR/BLxqUg0vM8dzo6cfrgXudJyYKuAQ/iIuyIU2RDwWN+Dd2DXkyMdCBKU5XbnZ1Aap4HTZVBuWPuwzvFphX0vLw8a+WAbTOunM+jpG42+hEijQRjWsx7h+ZxmJ//zzyn7/XKyko/14RMYejPB+at3vh0ytL1z4+WNmPZ2tUZ676aPXo93Pn4XGxaSlgJV5qan4JsOQBX4V9om1LahhaSNt7q6lmD9s4NRKlmC7WsIBszhOtiwdh0kavgJ5SbgktnvobMQjdXd3AvUSDw+DZDV7+FunMXqfgJ6Noh1J7bb+yF+RO+ghnuKDj6vPDZnwEs56jNK7hU8XND7sKFMQhES6g8dR857hyogXe5bocu5yPa34H+aBk1H/SHo2PQ9E+mcbvNcJDUF+Bz3oMmN0PBFEjyYVppF63YhAHrDng9v+W4AZpaRaY1mKSkAgEbHih3oQ+sNQSJK3OMMHYFy8p4cREeWI7wt4xRdRMqlqDm9A26cAddvI/rU7jOa03aQwVn07Lr0BlwM0ZZPOgfYjBaWBS8z434NcZP+AnXxWb8Caauwm28MbZA1yvZ74JfKoe1pwW9E1U0lXngKnqBSmXy0NRx38VY3TqkywiPJygntsNVUNAIq5sWcOU/xekWsllBRSyQdPKTYsl1PecLiJdGK35FEA1BWMoJmvEoaNIVF8tm6cVpfr4T/XI2NCme1lKgeS6g7tNWgQiUyHCXySgrC5jzz/dXAm+Vx4sUig0dLBNhvMYYZi8YCPeJfnQLPUzmOC8vYpgS4lSyCYVMpYL0JsoIrlgPwmfJCsUz8B9ZCHIZ1Y/gZRb9kQfhWMj+yF5o6LgK/gqBOwKheCOrj44MvOBX6KnulalIzP478erZLrHVDvVXI3OXvMaxAJ21VQIy3NMgWX7F6fPIKvqDuVUSInho7Cp8g7qugqbtNGgEneBBiMx1b2d3lS1UVj2Sck4auph4rCyFS8wA3rzsG9/9zaoNxSw6ZVh5UANWO/raW/HDl15F7tKspKrjh1qQVfBNXmVvAmoG81s8b4kzHK9FzdnjxntDSBfvFlfRWh6YY3ygFPJh0gndIh5UL6O2Ynfumk2JVZ/UNG/703ZET54Gxe+Fn9Gl6hpOHi7Fx7t/9z3i7hS6KejrC365pExKQHJukc89BcpkYtQHoP+jttmCvsv9kVGLHMj64hReVW8xB66kwO+gumIzFTnA0N1G9OPDp5oTwjZabh/qKs4iq/B1zo8y5ewWPCKtEQ7BM2JOXuTyzCR1ngKpgwjl9xBQPqpgojfOAIRuMqKjh2uziw98mBsBeerdO7L38NvyfAVyDK8Lgt/b05PMq+oCMhdvpKBi3h4HaM259MSPuJ+FjCKR90zIKljEgQuy/GOIXKla9hs0WUWbgIHzPk9vEvf9sbqKTMrwlb5NmbfldKcuV1OHYaBucogFEaNY4PURwW6DLWWmoZpfFQpGOmcnz7xMlx2EpO01GMjyGn7oIby4oolKVkPWRmpCXeJYq+I1eJMFxSHufdmg0fU9xD00KynlChDhDGiqkfGtM2ZSpgMenx+x1GEYDAsOz4BUuwWVXV5c0iPQlVuAj1v60eEXCFbpjt8nISlqKyd+uApWsS9nuCeg9P0J7KsZl1kC0wTdxb4aGc9NYJ9Ih51mASGU9CEpZqvBi0VOG3l/crcfPbnPolpzoqrbixnUIRTMnDa0IvGLYHPi/U4de+s70KA7YbUIgh5dG9StWLhQqHudLY3u62Rvh6pKjG+Jfwmcm6BLCcaTzuEVjxTi6B20fio3r+PkSZ/Biy8FO3lfUZ3YQ1mnuhhpdicdZNYkJiPgoYraTymRkoY1qdGYYotGHSu3k80iRB1SVEK8B8fKxLsjk6VWFaRAGg/HADwR/bD6z3LcFGRKNh9wfBtt3n5MsPRzngZNvwiZls1dGhu11O0RPAP8D0RBnIyM5KloZ2SdavPRPdIwGzF4yIJXBwPImWhDcncztCP7kBsJJNgEgUeL6Wx1sgBlXkMr6s6cZXphOa83GpWLLLFicZpFq+AqRfyMnC/jzvlBymuELG3gaa6gVdtYhu0yeJGn4C1kaEf2I6nrDrIpu8Hz8JUdeor1SJp80EsvKlYoyTMgUH18jDPQPY2t92ZxkMJvWomM/Nl02QaWWr8wEjCYC2VPIvdNkAaSiHcGmcunUimh+CbMW5zO4vY50qRca+sQ1vcEyFuURUrydIa5DYMeH6KMkDKTucgwoadYTrHpqOoaCFxWYgMPFi0JlLcNBO55+MhFvB1RcTeZKhZRSDdvkfNkcZzvjPeYZSdxvBzpU0VsmjAn4Rr0wHJWNfGoPfMe3XsCVu0cJG83eeTrkbG3WNva73p8WnnboCHrkiUmcLF7MDDdLD1Mz/IUK8N5MH7+wPX6K/jo8H5HqU/FA68P02KioPXy2ZueYm1pvT1oSNdUN912kQXoWibsr9MEW/DisnSUlAhjmAEkbpLi4nLU320gzk+p1Gr2H9JMi4nzjsErfYb1078dkStj4uS23j5EMrVNtCnK9frLtMfcAXTwFhzK0QbTRPfqeKQ+vZMMREkv/vcn2kG2o44c90vsBYgvM79OVMzZS3QKfsXYCa6bExMns+BVAyezYH0IjrHnyFnyMteOsgkZQVlHhA5JC9ZNGsIfPjHDg6GNz+iGHjyugl+zUNgRghjKY2QscLIKf2nijf2xRHyDxwgjseB2P5wlg9InT9ZH3bOCTpyeIIyei/XRaw/PxcuuvV2sPQrmAy2U/6M4Y1gRzIW7Hi/EZDAWnDGI+h9G+TdjiZcSp5HDrQAAAABJRU5ErkJggg==\"},";
    JSON += "{\"id\":5,\"name\":\"security\",\"data\":\"image/png;base64\",\"base64\":\"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\"},";
    JSON += "{\"id\":6,\"name\":\"garage\",\"data\":\"image/png;base64\",\"base64\":\"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\"},";
    JSON += "{\"id\":7,\"name\":\"temp\",\"data\":\"image/png;base64\",\"base64\":\"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\"}";
    JSON += "]";

    webServer.send(200, "text/json", JSON);
  });

  // Rest Repository ----------------------------------
  webServer.on("/rest", []() {
    String JSON = "{";
    JSON += "\"info\":\"www.homekitblogger.de\",";
    JSON += "\"status\":\"" + String(controllerData.status) + "\",";
    JSON += "\"restartrequired\":" + String(controllerData.restartrequired) + ",";
    JSON += "\"version\":\"" + String(controllerData.version) + "\",";
    JSON += "\"controller\":{";
    JSON += " \"chip_model\":\"" + String(ESP.getChipModel()) + "\",";
    JSON += " \"chip_revision\":\"Rev " + String(ESP.getChipRevision()) + "\",";
    JSON += " \"chip_core\":\"" + String(ESP.getChipCores() == 1 ? "single" : "dual") + "-core\",";
    JSON += " \"chip_flash\":\"" + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB\",";
    JSON += " \"runtime_sec\":" + String(millis() / 100) + ",";
    JSON += " \"free_heap\":" + String(esp_get_free_internal_heap_size()) + ",";
    JSON += " \"min_free_heap\":" + String(esp_get_minimum_free_heap_size()) + ",";
    JSON += " \"heap_total\":" + String(ESP.getHeapSize()) + ",";
    JSON += " \"heap_free\":" + String(ESP.getFreeHeap()) + ",";
    JSON += " \"psram_found\":" + String(controllerData.board_psram_found) + ",";
    JSON += " \"psram_total\":" + String(ESP.getPsramSize()) + ",";
    JSON += " \"psram_free\":" + String(ESP.getFreePsram()) + "";
    JSON += "},";
    JSON += "\"wifi\":{";
    JSON += " \"ssid\":\"" + String(WiFi.SSID()) + "\",";
    JSON += " \"rssi\":\"" + String(WiFi.RSSI()) + "\",";
    JSON += " \"mac\":\"" + String(WiFi.macAddress()) + "\",";
    JSON += " \"gateway\":\"" + WiFi.gatewayIP().toString() + "\"";
    JSON += "},";
    JSON += "\"homekit\":{";
    JSON += " \"name\":\"" + String(controllerData.homekit_name) + "\",";
    JSON += " \"type\":\"" + String(controllerData.homekit_type) + "\",";
    JSON += " \"maximumdevices\":" + String(controllerData.homekit_maxdevices) + ",";
    JSON += " \"activedevices\":" + String(devicesActive()) + ",";
    JSON += " \"code\":" + String(controllerData.homekit_code) + ",";
    JSON += " \"port\":" + String(controllerData.homekit_port) + ",";
    JSON += " \"hostname\":\"" + String(controllerData.homekit_hostnamebase) + String(controllerData.homekit_hostnamesuffix) + ".local\",";
    JSON += " \"modelname\":\"" + String(controllerData.homekit_modelname) + "\",";
    JSON += " \"qrcode\":\"" + String(controllerData.homekit_qrcode) + "\"";
    JSON += "},";
    JSON += "\"board\":{";
    JSON += " \"name\":\"" + String(controllerData.board_name) + "\",";
    JSON += " \"psram_found\":" + String(controllerData.board_psram_found) + ",";
    JSON += " \"pin_ap\":" + String(controllerData.board_pin_ap) + ",";
    JSON += " \"pin_i2c_sda\":" + String(controllerData.board_pin_i2c_sda) + ",";
    JSON += " \"pin_i2c_scl\":" + String(controllerData.board_pin_i2c_scl) + ",";
    JSON += " \"pin_spi_mosi\":" + String(MOSI) + ",";
    JSON += " \"pin_spi_clk\":" + String(SCK);
    JSON += "},";
    JSON += "\"homekit_devices\":[";
    for (int i = 0; i < aDEVICES; i++) {
      if (i > 0) JSON += ",";
      JSON += "{\"id\":" + String(i + 1) + ", \"active\":" + String(deviceData[i].active) + ", \"restartrequired\":" + String(deviceData[i].restartrequired) + ", \"homekit_id\":\"" + String(deviceData[i].homekitid) + "\",";
      JSON += " \"type\":{\"name\":\"" + String(deviceData[i].type.name) + "\", \"code\":\"" + String(deviceData[i].type.code) + "\", \"pintype\":\"" + String(deviceData[i].type.pintype) + "\", \"picture_id\":" + String(deviceData[i].type.picture_id) + "},";
      JSON += " \"name\":\"" + String(deviceData[i].name) + "\",";
      JSON += " \"text_1\":\"" + String(deviceData[i].text_1) + "\",\"text_2\":\"" + String(deviceData[i].text_2) + "\",\"text_3\":\"" + String(deviceData[i].text_3) + "\",";
      JSON += " \"pin_1\":" + String(deviceData[i].pin_1) + ", \"pin_2\":" + String(deviceData[i].pin_2) + ", \"pin_3\":" + String(deviceData[i].pin_3) + ", \"pin_4\":" + String(deviceData[i].pin_4) + ",";
      JSON += " \"pin_1_reverse\":" + String(deviceData[i].pin_1_reverse) + ", \"pin_2_reverse\":" + String(deviceData[i].pin_2_reverse) + ", \"pin_3_reverse\":" + String(deviceData[i].pin_3_reverse) + ", \"pin_4_reverse\":" + String(deviceData[i].pin_4_reverse) + ",";
      JSON += " \"bool_1\":" + String(deviceData[i].bool_1) + ",";
      JSON += " \"float_1\":" + String(deviceData[i].float_1) + ", \"float_2\":" + String(deviceData[i].float_2) + ",";
      JSON += " \"state\":\"" + String(deviceData[i].state_text) + "\", \"marked\":" + String(deviceData[i].state_marked) + ", \"error_last\":\"" + String(deviceData[i].error_last) + "\"}";
    }
    JSON += "],";
    JSON += "\"pins\":[";
    bool firstrow = true;
    bool showme = true;
    for (int pinnum = 0; pinnum < 40; pinnum++) {
      showme = true;
      if (controllerData.board_pin_ap==pinnum) showme = false;
      if (controllerData.board_pin_i2c_sda==pinnum) showme = false;
      if (controllerData.board_pin_i2c_scl==pinnum) showme = false;
      if (MOSI==pinnum) showme = false;
      if (SCK==pinnum) showme = false;
      for (int devnum = 0; devnum < aDEVICES; devnum++) {
        if (deviceData[devnum].pin_1==pinnum) showme = false;
        if (deviceData[devnum].pin_2==pinnum) showme = false;
        if (deviceData[devnum].pin_3==pinnum) showme = false;
        if (deviceData[devnum].pin_4==pinnum) showme = false;
      }
      if (showme) {
        if (!firstrow) JSON += ",";
        JSON += String(pinnum);
        firstrow = false;
      }
    }
    JSON += "],";
    JSON += "\"types\":[";
    firstrow = true;
    for (int i = 0; i < aTYPES; i++) {
      showme = true;
      if (!typeData[i].active) showme = false;
      if (typeData[i].onlyonce) {
        for (int x = 0; x < aDEVICES; x++) {
          if (deviceData[x].active && strcmp(deviceData[x].type.code, typeData[i].code) == 0) showme = false;
        }
      }
      if (showme) {
        if (!firstrow) JSON += ",";
        JSON += "{\"name\":\"" + String(typeData[i].name) + "\",\"code\":\"" + String(typeData[i].code) + "\"}";
        firstrow = false;
      }
    }
    JSON += "]}";

    webServer.send(200, "text/json", JSON);
  });

  // Rest State ----------------------------------
  webServer.on("/state", []() {
    String JSON = "{";
    JSON += "\"info\":\"www.homekitblogger.de\",";
    JSON += "\"status\":\"" + String(controllerData.status) + "\",";
    JSON += "\"runtime_sec\":" + String(millis() / 1000) + ",";
    JSON += "\"wifi_rssi\":\"" + String(WiFi.RSSI()) + "\",";
    JSON += "\"heap_total\":" + String(ESP.getHeapSize()) + ",";
    JSON += "\"heap_free\":" + String(ESP.getFreeHeap()) + ",";
    JSON += "\"psram_total\":" + String(ESP.getPsramSize()) + ",";
    JSON += "\"psram_free\":" + String(ESP.getFreePsram()) + ",";
    JSON += "\"devices_state\":[";
    for (int i = 0; i < aDEVICES; i++) {
      if (i > 0) JSON += ",";
      JSON += "{\"id\":" + String(i + 1) + ", \"active\":" + String(deviceData[i].active) + ", \"state\":\"" + String(deviceData[i].state_text) + "\", \"marked\":" + String(deviceData[i].state_marked) + "}";
    }
    JSON += "]";
    JSON += "}";

    webServer.send(200, "text/json", JSON);
  });

  // Rest Logging ----------------------------------
  webServer.on("/logging", []() {
    String JSON = "{";
    JSON += "\"log\":[";
    for (int i = 0; i < aLogging; i++) {
      if (i > 0) JSON += ",";
      JSON += "{\"position\":" + String(i) + ",\"runtime_sec\":" + String(actionLogging[i].runtime_sec) + ", \"devicename\":\"" + String(actionLogging[i].devicename) + "\", \"homekitid\":\"" + String(actionLogging[i].homekitid) + "\", \"action\":\"" + String(actionLogging[i].action) + "\"}";
    }
    JSON += "]";
    JSON += "}";

    webServer.send(200, "text/json", JSON);
  });

  // Save ----------------------------------
  webServer.on("/devicesave", []() {
    // device_id=0&device_name=none&device_type=light&device_pin=0&device_dimmable=0
    uint8_t device_id = 0;
    bool success = true;

    String content = "";
    // Get device id
    if (webServer.hasArg("device_id")) {
      device_id = webServer.arg("device_id").toInt();
    }
    // Check if new device is possible
    if (device_id == 0) {
      if (devicesActive() >= controllerData.homekit_maxdevices) {
        success = false;
        content = "Maximum number of allowed devices exceeded";
      }
    }
    if (success) {
      // In case of device id 0 (new device) find first new device
      if (device_id == 0) {
        // find first new device
        for (int i = 0; i < aDEVICES; i++) {
          if (deviceData[i].active == false) {
            device_id = i + 1;
            content += "Found next free device id: " + String(device_id) + "<br>";
            break;
          }
        }
      }
      // Fill device information
      for (int i = 0; i < webServer.args(); i++) {
        if (webServer.argName(i) == "device_name") {
          if (webServer.arg(i).length() >= 16) {success=false; content = "name can be up to 15 chars";}
          else strcpy(deviceData[device_id - 1].name, webServer.arg(i).c_str());
        } else if (webServer.argName(i) == "device_type") {
          if (webServer.arg(i).length() >= 11) {success=false; content = "type can be up to 10 chars";}
          else {
            for (int typ = 0; typ < aTYPES; typ++) {
              if (strcmp(typeData[typ].code, webServer.arg(i).c_str()) == 0) deviceData[device_id - 1].type = typeData[typ];
            }
          }
        } else if (webServer.argName(i) == "device_pin_1") {
          if (!isValidNumber(webServer.arg(i))) {success=false; content = "pin_1 must be numeric";}
          else deviceData[device_id - 1].pin_1 = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_2") {
          if (!isValidNumber(webServer.arg(i))) {success=false; content = "pin_2 must be numeric";}
          else deviceData[device_id - 1].pin_2 = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_3") {
          if (!isValidNumber(webServer.arg(i))) {success=false; content = "pin_3 must be numeric";}
          else deviceData[device_id - 1].pin_3 = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_4") {
          if (!isValidNumber(webServer.arg(i))) {success=false; content = "pin_4 must be numeric";}
          else deviceData[device_id - 1].pin_4 = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_1_reverse") {
          deviceData[device_id - 1].pin_1_reverse = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_2_reverse") {
          deviceData[device_id - 1].pin_2_reverse = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_3_reverse") {
          deviceData[device_id - 1].pin_3_reverse = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_pin_4_reverse") {
          deviceData[device_id - 1].pin_4_reverse = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_text_1") {
          if (webServer.arg(i).length() >= 17) {success=false; content = "text_1 can be up to 16 chars";}
          else strcpy(deviceData[device_id - 1].text_1, webServer.arg(i).c_str());
        } else if (webServer.argName(i) == "device_text_2") {
          if (webServer.arg(i).length() >= 17) {success=false; content = "text_2 can be up to 16 chars";}
          strcpy(deviceData[device_id - 1].text_2, webServer.arg(i).c_str());
        } else if (webServer.argName(i) == "device_text_3") {
          if (webServer.arg(i).length() >= 17) {success=false; content = "text_3 can be up to 16 chars";}
          strcpy(deviceData[device_id - 1].text_3, webServer.arg(i).c_str());
        } else if (webServer.argName(i) == "device_bool_1") {
          deviceData[device_id - 1].bool_1 = webServer.arg(i).toInt();
        } else if (webServer.argName(i) == "device_float_1") {
          if (!isValidNumber(webServer.arg(i))) {success=false; content = "float_1 must be numeric";}
          deviceData[device_id - 1].float_1 = webServer.arg(i).toFloat();
        } else if (webServer.argName(i) == "device_float_2") {
          if (!isValidNumber(webServer.arg(i))) {success=false; content = "float_2 must be numeric";}
          deviceData[device_id - 1].float_2 = webServer.arg(i).toFloat();
        }
      }
    }
    if(success) {
      // Set homekit id
      strcpy(deviceData[device_id - 1].homekitid, "");
      strcat(deviceData[device_id - 1].homekitid, deviceData[device_id - 1].type.code);
      strcat(deviceData[device_id - 1].homekitid, String(device_id).c_str());
      // Set device active
      deviceData[device_id - 1].active = true;
      // Set restart required
      deviceData[device_id - 1].restartrequired = true;

      // Save to non volatile storage
      writeToNVS();
    }

    webServer.send(200, "text/json", "{\"success\":" + String(success) + ",\"message\":\"" + content + "\"}");
  });

  // Delete ----------------------------------
  webServer.on("/devicedelete", []() {
    uint8_t device_id = 0;
    bool success = true;
    String content = "";

    if (webServer.hasArg("device_id")) {
      device_id = webServer.arg("device_id").toInt();
    }
    // Delete Device
    resetDevice(device_id - 1);

    // Save to non volatile storage
    writeToNVS();

    webServer.send(200, "text/json", "{\"success\":" + String(success) + ",\"message\":\"" + content + "\"}");
  });

// Save BOARD ----------------------------------
  webServer.on("/boardsave", []() {
    bool success = true;
    String content = "";
  
    // Configure Board
    for (int i = 0; i < webServer.args(); i++) {
      if (webServer.argName(i) == "board_pin_ap") {
        if (!isValidNumber(webServer.arg(i))) {success=false; content = "board_pin_ap must numeric";}
        else controllerData.board_pin_ap = webServer.arg(i).toInt();
      }
      if (webServer.argName(i) == "board_pin_i2c_sda") {
        if (!isValidNumber(webServer.arg(i))) {success=false; content = "board_pin_i2c_sda must numeric";}
        else controllerData.board_pin_i2c_sda = webServer.arg(i).toInt();
      }
      if (webServer.argName(i) == "board_pin_i2c_scl") {
        if (!isValidNumber(webServer.arg(i))) {success=false; content = "board_pin_i2c_scl must numeric";}
        else controllerData.board_pin_i2c_scl = webServer.arg(i).toInt();
      }
    }

    if(success) {
      // Save to non volatile storage
      writeToNVS();
    }
    // Respond
    webServer.send(200, "text/html", "{\"success\":" + String(success) + ",\"message\":\"" + content + "\"}");
    if(success) {
      delay(1000);  // Wait needed to have time for the webserver response!
      restartController();
    }
  });

  // Save CONTROLLER ----------------------------------
  webServer.on("/controllersave", []() {
    bool success = true;
    String content = "";
  
    // Configure Controller
    for (int i = 0; i < webServer.args(); i++) {
      if (webServer.argName(i) == "controller_homekit_name") {
        if (webServer.arg(i).length() <= 19) strcpy(controllerData.homekit_name, webServer.arg(i).c_str());
        else {success=false; content = "name can be up to 19 chars";}
      }
      if (webServer.argName(i) == "controller_homekit_type") {
        if (webServer.arg(i) == "bridge") {  // Type: Bridge
          strcpy(controllerData.homekit_type, "bridge");
          controllerData.homekit_maxdevices = homekit_maxdevices_bridge;
        } else {  // Fallback: Device
          strcpy(controllerData.homekit_type, "device");
          controllerData.homekit_maxdevices = homekit_maxdevices_device;
        }
      }
      if (webServer.argName(i) == "controller_homekit_code") {
        if (webServer.arg(i).length() != 8) {success=false; content = "code must be 8 chars";}
        else if (!isValidNumber(webServer.arg(i))) {success=false; content = "code must numeric";}
        else strcpy(controllerData.homekit_code, webServer.arg(i).c_str());
      }
      if (webServer.argName(i) == "controller_homekit_port") {
        if (webServer.arg(i).length() <= 2) {success=false; content = "port must more than 2 chars";}
        else if (webServer.arg(i).length() >= 6) {success=false; content = "port can be up to 5 chars";}
        else if (!isValidNumber(webServer.arg(i))) {success=false; content = "port must numeric";}
        else controllerData.homekit_port = webServer.arg(i).toInt();
      }
    }

    if(success) {
      // Reset Devices
      resetAllDevices();
      // Save to non volatile storage
      writeToNVS();
    }
    // Respond
    webServer.send(200, "text/html", "{\"success\":" + String(success) + ",\"message\":\"" + content + "\"}");
    if(success) {
      delay(1000);  // Wait needed to have time for the webserver response!
      unpairController();
      delay(1000);
      restartController();
    }
  });

  // Unpair ----------------------------------
  webServer.on("/unpair", []() {
    webServer.send(200, "text/html", "{\"success\":1,\"message\":\"unpair\"}");
    delay(1000);  // Wait needed to have time for the webserver response!
    unpairController();
    delay(1000);
    restartController();
  });

  // Reboot ----------------------------------
  webServer.on("/reboot", []() {
    webServer.send(200, "text/html", "{\"success\":1,\"message\":\"reboot\"}");
    delay(1000);  // Wait needed to have time for the webserver response!
    restartController();
  });

  // Reset ----------------------------------
  webServer.on("/reset", []() {
    webServer.send(200, "text/html", "{\"success\":1,\"message\":\"reset\"}");
    delay(1000);  // Wait needed to have time for the webserver response!
    resetController();
  });

  // Access Point ----------------------------------
  webServer.on("/ap", []() {
    startAccessPoint();
  });

}  // setupWeb

//////////////////////////////////////

// HELPER Controller
void statusUpdate(HS_STATUS status) {
  controllerData.status = homeSpan.statusString(status);
  logEntry(controllerData.homekit_name,(char*)controllerData.homekit_serialnumber,controllerData.status);
}
void unpairController() {
  homeSpan.processSerialCommand("U");  // unpair device by deleting all Controller data
}
void resetController() {
  unpairController();
  resetAllDevices();
  delay(1000);
  strcpy(controllerData.homekit_name, "HomeKit Controller"); // HOMEKIT name - shown in HomeKit
  strcpy(controllerData.homekit_type, "device");  // HOMEKIT type - bridge/device
  controllerData.homekit_maxdevices = 1;     // HOMEKIT maximum devices
  strcpy(controllerData.homekit_code, "11122333");  // HOMEKIT code
  controllerData.homekit_port = 1201;        // HOMEKIT port
  strcpy(controllerData.board_name, "");   // BOARD setup
  strcpy(controllerData.homekit_hostnamesuffix, "");   // HOST suffix
  strcpy(controllerData.homekit_qrcode, "");   // QR code
  controllerData.restartrequired = false; 
  controllerData.board_pin_ap = 15;       // BOARD Access Point Pull Up this pin to activate the Access Point. 15 RECOMMENDED
  controllerData.board_pin_i2c_sda = 21;  // BOARD I2C Pin for SDA
  controllerData.board_pin_i2c_scl = 22;  // BOARD I2C Pin for SCL
  writeToNVS();
  // Reset Homespan - factory reset and restart
  homeSpan.processSerialCommand("F");
}
void restartController() {
  for (int j = 0; j < 40; j++)  // this seems to be needed to ensure all pins are disconnected from led PWM on reboot
    gpio_reset_pin((gpio_num_t)j);  // otherwise ESP32 seems to be retaining some info about pins connectivity?

  ESP.restart();
}

// HELPER Devices
int devicesActive() {
  int counter = 0;
  for (int i = 0; i < aDEVICES; i++) {
    if (deviceData[i].active == true) counter++;
  }
  return counter;
}
void resetDevice(int position) {
  deviceData[position].active = false;
  strcpy(deviceData[position].name, "none"); 
  strcpy(deviceData[position].homekitid, "none"); 
  deviceData[position].type = typeData[0];
  deviceData[position].pin_1 = 0;
  deviceData[position].pin_2 = 0;
  deviceData[position].pin_3 = 0;
  deviceData[position].pin_4 = 0;
  deviceData[position].pin_1_reverse = false;
  deviceData[position].pin_2_reverse = false;
  deviceData[position].pin_3_reverse = false;
  deviceData[position].pin_4_reverse = false;
  deviceData[position].bool_1 = false;
  deviceData[position].float_1 = 0.0;
  deviceData[position].float_2 = 0.0;
  strcpy(deviceData[position].state_text, "none"); 
  deviceData[position].state_marked = false;
  strcpy(deviceData[position].error_last, "");
  deviceData[position].restartrequired = false;
}
void resetAllDevices() {
  for (int i = 0; i < aDEVICES; i++) {
    resetDevice(i);
  }
}

// HELPER non volatile storage
void writeToNVS() {
  // Controller
  nvs_set_blob(controllerNVS, "CONTROLLERDATA", &controllerData, sizeof(controllerData));  // update data
  nvs_commit(controllerNVS);                                                               // commit to NVS
  // Devices
  nvs_set_blob(deviceNVS, "DEVICEDATA", &deviceData, sizeof(deviceData));  // update data
  nvs_commit(deviceNVS);                                                   // commit to NVS
}
void readFromNVS() {
  // Controller
  size_t lencon;
  nvs_open("CONTROLLER", NVS_READWRITE, &controllerNVS);                      // open NVS
  if (!nvs_get_blob(controllerNVS, "CONTROLLERDATA", NULL, &lencon))          // if data found
    nvs_get_blob(controllerNVS, "CONTROLLERDATA", &controllerData, &lencon);  // retrieve data
  // Devices
  size_t lendev;
  nvs_open("DEVICES", NVS_READWRITE, &deviceNVS);                 // open NVS
  if (!nvs_get_blob(deviceNVS, "DEVICEDATA", NULL, &lendev))      // if data found
    nvs_get_blob(deviceNVS, "DEVICEDATA", &deviceData, &lendev);  // retrieve data
}
// HELPER HomeSpan Wifi
void startAccessPoint() {
  webServer.stop();
  delay(1000);
  // Starts the HomeSpan Setup Access Point
  // SSID: Homespan-Setup
  // PWD: homespan
  // Landingpage: 192.168.4.1/hotspot-detect.html
  homeSpan.processSerialCommand("A");  // starts the HomeSpan Setup Access Point
}
// HELPER input
boolean isValidNumber(String str){
  for(byte i=0;i<str.length();i++) {
    if(isDigit(str.charAt(i))) return true;
  }
  return false;
}
// HELPER Mdns suffix
const char *const alphabets = "aeiouybcdfghjklmnpqrstvwxza";
char generateLetter() {
  uint8_t index = random(0, 26);              //Generates random number between 0 and 26
  return (alphabets[index]);
}
void generateMdnsHostNameSuffix() {
  if (strcmp(controllerData.homekit_hostnamesuffix, "") == 0) { // Wenn suffix leer
    char spacer = '-';
    controllerData.homekit_hostnamesuffix[0] = spacer;
    controllerData.homekit_hostnamesuffix[1] = generateLetter();
    controllerData.homekit_hostnamesuffix[2] = generateLetter();
    controllerData.homekit_hostnamesuffix[3] = generateLetter();
    controllerData.homekit_hostnamesuffix[4] = generateLetter();
    controllerData.homekit_hostnamesuffix[5] = generateLetter();
    controllerData.homekit_hostnamesuffix[6] = generateLetter();
    writeToNVS();
  }
}
