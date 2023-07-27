


// Structures for action logging
enum logKind {INIT=0, STATE=1, TEMP=2, HUM=3, LUX=4, BATT=5, BRIGHT=6, ERROR=7};
char * logKindStr[8] = {"init", "state", "temp", "hum", "lux", "batt", "bright", "error"};
#define aLogging 20                 // maximum number of loggings
struct {   
  long int runtime_sec = 0;
  char name[21] = "";
  logKind kind = logKind::INIT;
  String value = ""; 
  char unit[4] = "";
  bool mqtt_send = true;
} actionLogging[aLogging];

// Define device types
#define aTYPES 21  // maximum number of devices
struct structType {
  bool active;
  char name[21];
  char code[11];
  bool onlyonce; // Only once per Controller
  char pintype[6]; //GPIO,ADC1,ADC2,TOUCH};
  uint8_t accessory_count; // How many SpanAccessory within the Device
  uint8_t picture_id;
};
const structType typeData[aTYPES] = {
  {false,"", "",false, "",1,0},
  {false,"Battery (MODBUS)", "batmodbus",false, "",1,0},
  {true,"Button", "button",false, "GPIO",1,2},
  {true,"Contact", "contact",false, "GPIO",1,4},
  {true,"Doorbell", "doorbell",false, "GPIO",1,2},
  {false,"Garage Door", "garage",false, "GPIO",1,6},
  {false,"Leak", "leak",false, "GPIO",1,0},
  {true,"Led", "led",false, "",1,1},
  {true,"Led (MAX7219)", "maxled",true, "GPIO",4,1},
  {true,"Led (RGB)", "rgbled",false, "",1,1},
  {true,"Light (BH1750)", "bh1750",true, "I2C",1,0},
  {true,"Light (TEMT6000)", "temt6000",false, "ADC1",1,0},
  {true,"Motion (HC-SR501)", "hcsr501",false, "GPIO",1,4},
  {true,"Motion (SW420)", "sw420",false, "GPIO",1,4},
  {false,"Outlet", "outlet",false, "GPIO",1,0},
  {true,"Security", "security",false, "GPIO",1,5},
  {true,"Security (NOLOGIC)", "terxon",false, "GPIO",1,5},
  {true,"Smoke (MQ-2)", "mq2",false, "GPIO",1,0},
  {true,"Temp. (DS18B20)", "ds18b20",false, "ADC2",1,7},
  {true,"Temp. (DHT11)", "dht11",false, "ADC2",1,7},
  {true,"Temp. (DHT22)", "dht22",false, "ADC2",1,7}
};

// Structure for controller settings
struct {
  const char* version = "1.0.1";            // Software Version
  bool restartrequired = false;  // Controller Restart Required
  // HOMEKIT
  char homekit_name[20] = "HomeSpanUI";    // HOMEKIT name - shown in HomeKit
  char homekit_type[7] = "device";    // HOMEKIT type - bridge/device
  uint8_t homekit_maxdevices = 1;   // HOMEKIT maximum devices
  char homekit_code[9] = "11122333";  // HOMEKIT code
  uint16_t homekit_port = 1201;      // HOMEKIT port
  const char* homekit_manufacturer = "homekitblogger.de";    // HOMEKIT manufacturer
  const char* homekit_serialnumber = "HomeSpanUI";    // HOMEKIT serialnumber
  const char* homekit_hostnamebase = "ESP";  // HOMEKIT the full MDNS host name is broadcast by HomeSpan
  char homekit_hostnamesuffix[7] = "";  
  const char* homekit_modelname = "ESP32HomeKit";  // HOMEKIT the HAP model name HomeSpan broadcasts for pairing to HomeKit
  const char* homekit_setupid = "HSUI";      // HOMEKIT Setup ID 4-chars
  char homekit_qrcode[21];             // HOMEKIT Text for QR Code
  // BOARD
  char board_name[13] = "";
  bool board_psram_found = false;   // BOARD psram found
  uint8_t board_pin_ap = 15;       // BOARD Access Point Pull Up this pin to activate the Access Point. 15 RECOMMENDED
  uint8_t board_pin_i2c_sda = 21;  // BOARD I2C Pin for SDA DATA
  uint8_t board_pin_i2c_scl = 22;  // BOARD I2C Pin for SCL CLOCK
  // MQTT
  bool mqtt_active = false;   // MQTT Active
  char mqtt_server[16] = "192.168.178.51";
  int mqtt_port = 1883;
  char mqtt_user[16] = "";
  char mqtt_password[16] = "";
} controllerData;

// Structure for devices settings
#define aDEVICES 8  // maximum number of devices
struct {
  bool active = false;
  char name[16] = "none";
  structType type = typeData[0];
  char homekitid[15] = "none";
  // Config
  char text_1[17] = "";
  char text_2[17] = "";
  char text_3[17] = "";
  uint8_t pin_1 = 0;
  uint8_t pin_2 = 0;
  uint8_t pin_3 = 0;
  uint8_t pin_4 = 0;
  bool pin_1_reverse = false;
  bool pin_2_reverse = false;
  bool pin_3_reverse = false;
  bool pin_4_reverse = false;
  bool bool_1 = false;
  float float_1 = 0.0;
  float float_2 = 0.0;
  // Status
  char state_1_value[15] = "";
  char state_1_unit[4] = "";
  char state_2_value[15] = "";
  char state_2_unit[4] = "";
  bool state_marked = false;
  uint8_t state_overrule = 0;
  // Miscs
  String error_last = "";
  bool restartrequired = false;
} deviceData[aDEVICES];


// Helper Log
void logEntry(char name[21], logKind kind, String value, char unit[4] = "") {
  for(int i=aLogging-1; i>=1; i--) {
    actionLogging[i].runtime_sec = actionLogging[i-1].runtime_sec;
    strcpy(actionLogging[i].name, actionLogging[i-1].name);
    actionLogging[i].kind = actionLogging[i-1].kind;
    actionLogging[i].value = actionLogging[i-1].value;
    strcpy(actionLogging[i].unit, actionLogging[i-1].unit);
    actionLogging[i].mqtt_send = actionLogging[i-1].mqtt_send;
  }
  actionLogging[0].runtime_sec = millis()/1000;
  strcpy(actionLogging[0].name, name);
  actionLogging[0].kind = kind;
  actionLogging[0].value = value;
  strcpy(actionLogging[0].unit, unit);
  actionLogging[0].mqtt_send = false;
}

enum {NFC=1,IP=2,BLTE=4};
char *getQrCodeText(uint32_t setupCode, const char *setupID, uint8_t category, uint8_t protocols=IP, uint8_t qVersion=0, uint8_t qReserved=0){   
      setupCode&=0x07FFFFFF;     // valid values: 0-99999999
      qVersion&=0x7;             // valid values: 0-7
      qReserved&=0xF;            // valid values: 0-15
      protocols&=0x7;            // valid values: 0-7
      
      uint64_t n=((uint64_t) qVersion<<43) | ((uint64_t) qReserved<<39) | ((uint64_t) category<<31) | (protocols<<27) | setupCode;
      sprintf(controllerData.homekit_qrcode,"X-HM://");
  
      for(int i=15;i>=7;i--){
        controllerData.homekit_qrcode[i]=n%36+48;
        if(controllerData.homekit_qrcode[i]>57)
          controllerData.homekit_qrcode[i]+=7;
        n/=36;
      }

      sprintf(controllerData.homekit_qrcode+16,"%-4.4s",setupID);
      return(controllerData.homekit_qrcode);
}
