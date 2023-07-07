// Noch zu programmieren: LockMechanism, Switch, IrrigationSystem (Watering)

// Classes
class_ledmatrix *ledMatrix;
// Log
enum logKind {INIT, CHANGES, ERROR};
void logDevice(uint8_t device_id, logKind log_kind, char message[31] = "") {
  switch (log_kind)
  {
    case INIT: {
      LOG1(String(deviceData[device_id].name) + " (" + String(deviceData[device_id].type.code) + " " + deviceData[device_id].homekitid + ") -> Init: " + deviceData[device_id].state_text + "\n");
      logEntry(deviceData[device_id].name,deviceData[device_id].homekitid,"init"); 
      break;
    }
    case CHANGES: {
      LOG1(String(deviceData[device_id].name) + " (" + String(deviceData[device_id].type.code) + " " + deviceData[device_id].homekitid + ") -> Change: " + deviceData[device_id].state_text + "\n");
      logEntry(deviceData[device_id].name,deviceData[device_id].homekitid,"changed to " + String(deviceData[device_id].state_text)); 
      break;
    }
    case ERROR: { 
      strcpy(deviceData[device_id].error_last, message); 
      LOG1(String(deviceData[device_id].name) + " (" + String(deviceData[device_id].type.code) + " " + deviceData[device_id].homekitid + ") -> Error: " + message + "\n");
      logEntry(deviceData[device_id].name,deviceData[device_id].homekitid,message); 
      break;
    }
  }
}

// Device: Accessory Information
struct deviceAccessoryInformation : Service::AccessoryInformation {

  int nBlinks;                    // number of times to blink built-in LED in identify routine
  SpanCharacteristic *identify;   // reference to the Identify Characteristic
  
  deviceAccessoryInformation(const char *name, const char *manu, const char *sn, const char *model, const char *version, int nBlinks) : Service::AccessoryInformation(){
    
    new Characteristic::Name(name);                   // create all the required Characteristics with values set based on above arguments
    new Characteristic::Manufacturer(manu);
    new Characteristic::SerialNumber(sn);    
    new Characteristic::Model(model);
    new Characteristic::FirmwareRevision(version);
    identify=new Characteristic::Identify();          // store a reference to the Identify Characteristic for use below

    this->nBlinks=nBlinks;                            // store the number of times to blink the LED

    pinMode(homeSpan.getStatusPin(),OUTPUT);          // make sure LED is set for output
  }

  boolean update(){
       
    for(int i=0;i<nBlinks;i++){
      digitalWrite(homeSpan.getStatusPin(),LOW);
      delay(250);
      digitalWrite(homeSpan.getStatusPin(),HIGH);
      delay(250);
    }

    return(true);                               // return true
    
  } // update
  
};

// ############ LIGHT DEVICES
// Device: Generic LED (ok)
struct deviceGenericLED : Service::LightBulb {       // Generic LED

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  LedPin *LED;                                      // reference to an LedPin
  // State
  bool poweron = false;
  String brightness = "100";
  
  SpanCharacteristic *power;                        // reference to the On Characteristic
  SpanCharacteristic *level;                        // reference to the Brightness Characteristic
 
  deviceGenericLED(int sDeviceid) : Service::LightBulb(){

    this->deviceid = sDeviceid;

    power=new Characteristic::On(0,true);           // second argument is true, so the value of the On Characteristic (initially set to 0) will be saved in NVS
    if(deviceData[this->deviceid].bool_1){
      level=new Characteristic::Brightness(50,true); // second argument is true, so the value of the Brightness Characteristic (initially set to 50) will be saved in NVS               
      level->setRange(5,100,1);                       // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
    }
    // Init state
    this->LED=new LedPin(deviceData[this->deviceid].pin_1);      // configures a PWM LED for output to pin number "ledPin"
    LED->set(power->getVal()*(deviceData[this->deviceid].bool_1?(level->getVal()):100));             // set the LED to its initial state at startup.
    float poweron = 0;
    float brightness = 100;
    if (this->poweron) strcpy(deviceData[this->deviceid].state_text, "on ");
    else  strcpy(deviceData[this->deviceid].state_text, "off ");
    if (deviceData[this->deviceid].bool_1) strcat(deviceData[this->deviceid].state_text, String(this->brightness).c_str());
    if (this->poweron) deviceData[this->deviceid].state_marked = true;
    else deviceData[this->deviceid].state_marked = false;
    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  boolean update(){                              // update() method
    // Get State
    if (power->getNewVal()) this->poweron = true;
    else this->poweron = false;
    if(deviceData[this->deviceid].bool_1){
      this->brightness = String(level->getNewVal());
    } 
    // Set state
    LED->set(power->getNewVal()*(deviceData[this->deviceid].bool_1?(level->getNewVal()):100));       // update the physical LED to reflect the new values 
    // Device state
    if (this->poweron) strcpy(deviceData[this->deviceid].state_text, "on ");
    else  strcpy(deviceData[this->deviceid].state_text, "off ");
    if (deviceData[this->deviceid].bool_1) strcat(deviceData[this->deviceid].state_text, String(this->brightness).c_str());
    if (this->poweron) deviceData[this->deviceid].state_marked = true;
    else deviceData[this->deviceid].state_marked = false;
    // Log
    logDevice(this->deviceid,logKind::CHANGES); 
    return(true);                               // return true 
  } // update

};
// Device: RGB LED (ok)
struct deviceRgbLED : Service::LightBulb {       // RGB LED (Command Cathode)

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  LedPin *redPin, *greenPin, *bluePin;       // reference to an LedPin 
  // State
  bool poweron = false;
  String state_rgb = "";

  SpanCharacteristic *power;                   // reference to the On Characteristic
  SpanCharacteristic *H;                       // reference to the Hue Characteristic
  SpanCharacteristic *S;                       // reference to the Saturation Characteristic
  SpanCharacteristic *V;                       // reference to the Brightness Characteristic
  
  deviceRgbLED(int sDeviceid) : Service::LightBulb(){       // constructor() method

    this->deviceid = sDeviceid;

    power=new Characteristic::On();                    
    H=new Characteristic::Hue(0);              // instantiate the Hue Characteristic with an initial value of 0 out of 360
    S=new Characteristic::Saturation(0);       // instantiate the Saturation Characteristic with an initial value of 0%
    V=new Characteristic::Brightness(100);     // instantiate the Brightness Characteristic with an initial value of 100%
    V->setRange(5,100,1);                      // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
    // Init state
    this->redPin=new LedPin(deviceData[this->deviceid].pin_1);        // configures a PWM LED for output to the RED pin
    this->greenPin=new LedPin(deviceData[this->deviceid].pin_2);    // configures a PWM LED for output to the GREEN pin
    this->bluePin=new LedPin(deviceData[this->deviceid].pin_3);      // configures a PWM LED for output to the BLUE pin
    float poweron = 0;
    this->state_rgb = "(0,0,0)";
    if (this->poweron) strcpy(deviceData[this->deviceid].state_text, "on ");
    else  strcpy(deviceData[this->deviceid].state_text, "off ");
    strcat(deviceData[this->deviceid].state_text, String(this->state_rgb).c_str());
    if (this->poweron) deviceData[this->deviceid].state_marked = true;
    else deviceData[this->deviceid].state_marked = false;
    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  boolean update(){                         // update() method
    // Get State
    boolean p;
    float v, h, s, r, g, b;
    h=H->getVal<float>();                      // get and store all current values.  Note the use of the <float> template to properly read the values
    s=S->getVal<float>();
    v=V->getVal<float>();                      // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately
    p=power->getVal();
    if(power->updated()) p=power->getNewVal();
    if(H->updated()) h=H->getNewVal<float>();
    if(S->updated()) s=S->getNewVal<float>();
    if(V->updated()) v=V->getNewVal<float>();
    LedPin::HSVtoRGB(h,s/100.0,v/100.0,&r,&g,&b);   // since HomeKit provides S and V in percent, scale down by 100
    int R, G, B;
    R=p*r*100;                                      // since LedPin uses percent, scale back up by 100, and multiple by status fo power (either 0 or 1)
    G=p*g*100;
    B=p*b*100;

    if (power->getNewVal()) this->poweron = true;
    else this->poweron = false;
    this->state_rgb = "(" + String(R) + "," + String(G) + "," + String(B) + ")";
    // Set state
    redPin->set(R);                      // update each ledPin with new values
    greenPin->set(G);    
    bluePin->set(B);    
    // Device state
    if (this->poweron) strcpy(deviceData[this->deviceid].state_text, "on ");
    else  strcpy(deviceData[this->deviceid].state_text, "off ");
    strcat(deviceData[this->deviceid].state_text, String(this->state_rgb).c_str());
    if (this->poweron) deviceData[this->deviceid].state_marked = true;
    else deviceData[this->deviceid].state_marked = false;
    // Log
    logDevice(this->deviceid,logKind::CHANGES);
    return(true);                               // return true 
  } // update
};
// Device: MAX7219 LED MATRIX (ok)
struct deviceMax7219LED : Service::LightBulb {       // Generic LED

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  int accessory_num = 0;

  SpanCharacteristic *power;                        // reference to the On Characteristic
 
  deviceMax7219LED(int sDeviceid,int sAccessoryNum) : Service::LightBulb(){

    this->deviceid = sDeviceid;
    this->accessory_num = sAccessoryNum;

    if (this->accessory_num==0) new Characteristic::Name("Sym 1");
    if (this->accessory_num==1) new Characteristic::Name("Sym 2");
    if (this->accessory_num==2) new Characteristic::Name("Sym 3");
    if (this->accessory_num==3) new Characteristic::Name("Off");
    power=new Characteristic::On();           // no parameter, no NVS storage
    // Init state
    strcpy(deviceData[this->deviceid].state_text, "Off");
    deviceData[this->deviceid].state_marked = false;
    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  boolean update(){  
    if (power->getNewVal()) { // If turn on
      // Set overrule
      deviceData[this->deviceid].state_overrule = this->accessory_num;
      // Set state
      setMatrix();
      // Device state
      if (this->accessory_num==0) strcpy(deviceData[this->deviceid].state_text, "Sym1");
      if (this->accessory_num==1) strcpy(deviceData[this->deviceid].state_text, "Sym2");
      if (this->accessory_num==2) strcpy(deviceData[this->deviceid].state_text, "Sym3");
      if (this->accessory_num==3) strcpy(deviceData[this->deviceid].state_text, "Off");
      if (this->accessory_num==0) deviceData[this->deviceid].state_marked = true;
      if (this->accessory_num==1) deviceData[this->deviceid].state_marked = true;
      if (this->accessory_num==2) deviceData[this->deviceid].state_marked = true;
      if (this->accessory_num==3) deviceData[this->deviceid].state_marked = false;
      // Log
      logDevice(this->deviceid,logKind::CHANGES);      
    }
    return(true);                               // return true 
  } // update

  void loop(){
    // check that power is true, and that time since last modification is greater than 3 seconds, and that i am not the overruling instance
    if(power->getVal() && power->timeVal()>3000 && (deviceData[this->deviceid].state_overrule!=this->accessory_num || this->accessory_num==3)){  
      power->setVal(false);                         // set power to false
    }      
  } // loop

  void setMatrix() {
    if (this->accessory_num==0 || this->accessory_num==1 || this->accessory_num==2) {
      ledMatrix->clear();    
      ledMatrix->commit();
      // Pacman Example byte actByte[8]={B00111100,B01001110,B11001100,B11111000,B11111000,B11111100,B01111110,B00111100}; 
      // Pacman Example HEX 3C,4E,CC,F8,F8,FC,7E,3C
      // Pacman Example Chars 3C4ECCF8F8FC7E3C
      // Stars 2070220722702000 Lightning 6030187C30180C00 emojismile 3C7EDBFFBDC37E3C emojiworried 3C7EDBFFC3BD7E3C skull 221C3E2A3E1C2200
      char hex_text[17] = "";
      if (this->accessory_num==0) strcpy(hex_text, deviceData[this->deviceid].text_1);
      if (this->accessory_num==1) strcpy(hex_text, deviceData[this->deviceid].text_2);
      if (this->accessory_num==2) strcpy(hex_text, deviceData[this->deviceid].text_3);

      char hexA[9];
      char hexB[9];
      for(int i=0;i<8;i++){
        hexA[i] = hex_text[i];
        hexB[i] = hex_text[i+8];
      }

      byte actByte[8];
      unsigned long numberA = strtoul( hexA, nullptr, 16);
      unsigned long numberB = strtoul( hexB, nullptr, 16);
      for(int i=3; i>=0; i--)    // start with lowest byte of number
      {
        actByte[i] = numberA & 0xFF;  // or: = byte( number);
        numberA >>= 8;            // get next byte into position
        actByte[i+4] = numberB & 0xFF;  // or: = byte( number);
        numberB >>= 8;            // get next byte into position
      }

      for(int i=0;i<8;i++){ // Per row
        if (bitRead(actByte[i], 0)==1) ledMatrix->setPixel(i,0);
        if (bitRead(actByte[i], 1)==1) ledMatrix->setPixel(i,1);
        if (bitRead(actByte[i], 2)==1) ledMatrix->setPixel(i,2);
        if (bitRead(actByte[i], 3)==1) ledMatrix->setPixel(i,3);
        if (bitRead(actByte[i], 4)==1) ledMatrix->setPixel(i,4);
        if (bitRead(actByte[i], 5)==1) ledMatrix->setPixel(i,5);
        if (bitRead(actByte[i], 6)==1) ledMatrix->setPixel(i,6);
        if (bitRead(actByte[i], 7)==1) ledMatrix->setPixel(i,7);
      }
      ledMatrix->commit();
    } else {
      ledMatrix->clear();    
      ledMatrix->commit();
    }
  }
};
// ############ BUTTON DEVICES
// Device: Programmable Button (ok)
struct deviceProgButton : Service::StatelessProgrammableSwitch {       // Stateless Programmable Switch

  // Settings
  bool initsuccess = false;
  int deviceid = 0;  

  SpanCharacteristic *switchEvent;                  // reference to the ProgrammableSwitchEvent Characteristic
  
  deviceProgButton(int sDeviceid, int index) : Service::StatelessProgrammableSwitch(){

    switchEvent=new Characteristic::ProgrammableSwitchEvent();  // Programmable Switch Event Characteristic (will be set to SINGLE, DOUBLE or LONG press)
    new Characteristic::ServiceLabelIndex(index);               // set service label index (only required if there is more than one Stateless Programmable Switch per Service)
    // Init State
    new SpanButton(deviceData[this->deviceid].pin_1);                                  // create new SpanButton
    strcpy(deviceData[this->deviceid].state_text, "waiting for press"); 
    deviceData[this->deviceid].state_marked = false;
    // Init Log
    logDevice(this->deviceid,logKind::INIT); 
    this->initsuccess = true;
  } // end constructor

  void button(int pin, int pressType) override {
    // Set state
    switchEvent->setVal(pressType);                // set the value of the switchEvent Characteristic
    // Device State
    if (pressType==0) { strcpy(deviceData[this->deviceid].state_text, "single press"); }
    if (pressType==1) { strcpy(deviceData[this->deviceid].state_text, "double press"); }
    if (pressType==2) { strcpy(deviceData[this->deviceid].state_text, "long press"); }
    // Log
    logDevice(this->deviceid,logKind::CHANGES);
    // Reset State
    strcpy(deviceData[this->deviceid].state_text, "waiting for press"); 
  }
};
// Device: DoorBell (ok)
struct deviceDoorBell : Service::Doorbell {  // A DoorBell
  
  // Settings
  bool initsuccess = false;
  int deviceid = 0;  

  SpanCharacteristic *switchEvent;                  // reference to the ProgrammableSwitchEvent Characteristic
  
  deviceDoorBell(int sDeviceid, int sIndex) : Service::Doorbell(){

    this->deviceid = sDeviceid;

    switchEvent=new Characteristic::ProgrammableSwitchEvent();  // Programmable Switch Event Characteristic (will be set to SINGLE, DOUBLE or LONG press)
    new Characteristic::ServiceLabelIndex(sIndex);               // set service label index (only required if there is more than one Stateless Programmable Switch per Service)
    // Init State
    new SpanButton(deviceData[this->deviceid].pin_1);                                  // create new SpanButton
    strcpy(deviceData[this->deviceid].state_text, "waiting for press"); 
    deviceData[this->deviceid].state_marked = false;
    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  void button(int pin, int pressType) override {
    // Set state
    switchEvent->setVal(pressType);                // set the value of the switchEvent Characteristic
    // Device State
    if (pressType==0) { strcpy(deviceData[this->deviceid].state_text, "single press"); }
    if (pressType==1) { strcpy(deviceData[this->deviceid].state_text, "double press"); }
    if (pressType==2) { strcpy(deviceData[this->deviceid].state_text, "long press"); }
    // Log
    logDevice(this->deviceid,logKind::CHANGES);
    // Reset State
    strcpy(deviceData[this->deviceid].state_text, "waiting for press"); 
  }
};
// ############ CONTACT SENSORS
// Device: Contact Sensor (ok)
struct deviceContactSensor : Service::ContactSensor {     // A ContactSensor
  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  bool opencontact = false;
  bool opencontact_old = false;
  // Timer
  float waitingtime = 100;   

  SpanCharacteristic *contact;            // reference to the SecuritySystemCurrentState Characteristic (specific to ContactSensor)

  deviceContactSensor(int sDeviceid) : Service::ContactSensor(){       // constructor() method
        
    this->deviceid = sDeviceid;

    contact=new Characteristic::ContactSensorState(1);              // initial value of 1 = open - 0 = closed
    // Init State
    pinMode(deviceData[this->deviceid].pin_1, INPUT_PULLDOWN); // Default state LOW
    this->opencontact = false;
    strcpy(deviceData[this->deviceid].state_text, "closed"); 
    deviceData[this->deviceid].state_marked = false;
    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  void loop(){                                     // loop() method
    // wait
    if (this->initsuccess && contact->timeVal() > this->waitingtime) {
      getContact();    
    }
  } // loop

  void getContact() {
    // Get state
    this->opencontact = true; // Default Open
    if (!deviceData[this->deviceid].pin_1_reverse && digitalRead(deviceData[this->deviceid].pin_1) == HIGH) this->opencontact = false;  // Closed
    if (deviceData[this->deviceid].pin_1_reverse && digitalRead(deviceData[this->deviceid].pin_1) == LOW) this->opencontact = false;  // Closed Reverse
    // If new state diff
    if (this->opencontact_old != this->opencontact) {
      // Set State
      contact->setVal(this->opencontact); // Set new homekit state 1 = open - 0 = closed
      this->opencontact_old = this->opencontact;
      // Device State
      if (this->opencontact) { strcpy(deviceData[this->deviceid].state_text, "open"); deviceData[this->deviceid].state_marked = true; }
      else { strcpy(deviceData[this->deviceid].state_text, "closed"); deviceData[this->deviceid].state_marked = false; }
      // Log
      logDevice(this->deviceid,logKind::CHANGES);
    }
  }  //getContact()
};
// Device: Security System (ok)
struct deviceSecuritySystem : Service::SecuritySystem {     // A Security System

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  uint8_t securitystate = 3; // Default: 3 Disarm (Stay=0 / Away=1 / Night=2 / Disarmed=3 / Triggered=4)
  uint8_t securitystate_old = 3;
  // Timer
  float waitingtime = 1000;    

  SpanCharacteristic *current;            // reference to the SecuritySystemCurrentState Characteristic (specific to SecuritySystem)
  SpanCharacteristic *target;             // reference to the SecuritySystemTargetState Characteristic (specific to SecuritySystem)  

  deviceSecuritySystem(int sDeviceid) : Service::SecuritySystem(){       // constructor() method
        
    this->deviceid = sDeviceid;
    
    if (strcmp(deviceData[this->deviceid].type.code, "security") == 0) {
      current=new Characteristic::SecuritySystemCurrentState(1);              // initial value of 3 means disarmed
      target=new Characteristic::SecuritySystemTargetState(1);                // initial value of 3 means disarmed
      // Init State
      pinMode(deviceData[this->deviceid].pin_1, INPUT_PULLDOWN); // Contact Default state LOW
      pinMode(deviceData[this->deviceid].pin_2, INPUT_PULLDOWN); // Armed Default state LOW
      this->securitystate = 3;
      strcpy(deviceData[this->deviceid].state_text, "disarmed"); 
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "terxon") == 0) {
      current=new Characteristic::SecuritySystemCurrentState(3);              // initial value of 3 means disarmed
      target=new Characteristic::SecuritySystemTargetState(3);                // initial value of 3 means disarmed
      // Init State
      pinMode(deviceData[this->deviceid].pin_1, INPUT_PULLDOWN); // Away Default state LOW
      pinMode(deviceData[this->deviceid].pin_2, INPUT_PULLDOWN); // Triggered Default state LOW
      pinMode(deviceData[this->deviceid].pin_3, INPUT_PULLDOWN); // Night Default state LOW
      pinMode(deviceData[this->deviceid].pin_4, INPUT_PULLDOWN); // Stay Default state LOW
      this->securitystate = 3;
      strcpy(deviceData[this->deviceid].state_text, "disarmed"); 
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } // end constructor

  void loop() {
    // wait
    if (this->initsuccess && current->timeVal() > this->waitingtime) {
      getSecurity();    
    }
  } // loop

  void getSecurity() {
    // Get state
    if (strcmp(deviceData[this->deviceid].type.code, "security") == 0) {
      if (digitalRead(deviceData[this->deviceid].pin_2) == HIGH) { // Closed - Armed - Away
        if (digitalRead(deviceData[this->deviceid].pin_1) == LOW) this->securitystate = 4; // Open - Triggered - Ausgeloest
        else  this->securitystate = 1; // Closed - Away - Abwesend
      }
      else this->securitystate = 3; // Open - Disarmed - Aus
    } 
    else if (strcmp(deviceData[this->deviceid].type.code, "terxon") == 0) {
      if (digitalRead(deviceData[this->deviceid].pin_2) == HIGH) this->securitystate = 4; // Triggered - Ausgeloest
      else if (digitalRead(deviceData[this->deviceid].pin_1) == HIGH) this->securitystate = 1; // Away - Abwesend
      else if (digitalRead(deviceData[this->deviceid].pin_3) == HIGH) this->securitystate = 2; // Night - Nacht
      else if (digitalRead(deviceData[this->deviceid].pin_4) == HIGH) this->securitystate = 0; // Stay - Zuhause
      else this->securitystate = 3; // Disarmed - Aus
    }
    // If new state diff
    if (this->securitystate_old != this->securitystate) { // if current-state matches target-state there is nothing do 
      // Set state
      if (this->securitystate == 4) { current->setVal(this->securitystate); }  // If state triggered
      else { target->setVal(this->securitystate); delay(200); current->setVal(this->securitystate); }   // If state normal
      this->securitystate_old = this->securitystate;
      // Device State
      if (this->securitystate==1) { strcpy(deviceData[this->deviceid].state_text, "away"); deviceData[this->deviceid].state_marked = true; }
      else if (this->securitystate==2) { strcpy(deviceData[this->deviceid].state_text, "night"); deviceData[this->deviceid].state_marked = true; }
      else if (this->securitystate==3) { strcpy(deviceData[this->deviceid].state_text, "disarmed"); deviceData[this->deviceid].state_marked = false; }
      else if (this->securitystate==4) { strcpy(deviceData[this->deviceid].state_text, "triggered"); deviceData[this->deviceid].state_marked = true; }
      else { strcpy(deviceData[this->deviceid].state_text, "stay"); deviceData[this->deviceid].state_marked = false; }
      // Log
      logDevice(this->deviceid,logKind::CHANGES);
    } 
  }
};
// ############ DYNAMIC SENSORS
// Device: Temperature Sensor (not tested)
struct deviceTemperatureSensor : Service::TemperatureSensor {      // A Temperature sensor 

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  float temperature = 0;
  float humidity = 0;
  // Timer
  float waitingtime = 10000;

  DallasTemperature *sensorsDallas;
  DHT *sensorsDht;

  SpanCharacteristic *temp;
  SpanCharacteristic *hum;                                         

  // constructor() method
  deviceTemperatureSensor(int sDeviceid) : Service::TemperatureSensor() {      

    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "ds18b20") == 0) { // Init OneWire and Dallas Pointers
      temp=new Characteristic::CurrentTemperature(this->temperature);       // instantiate the Current Temperature Characteristic
      temp->setRange(-50,250);                                  // expand the range from the HAP default of 0-100 to -50 to 150 to allow for negative temperatures
      // Init state
      OneWire *oneWirePtr;
      oneWirePtr =  new OneWire(deviceData[this->deviceid].pin_1);
      sensorsDallas = new DallasTemperature(oneWirePtr);
      sensorsDallas->begin();
      strcpy(deviceData[this->deviceid].state_text, String(this->temperature).c_str());
      strcat(deviceData[this->deviceid].state_text, " C");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "dht11") == 0) { // Init DHT Pointers
      temp = new Characteristic::CurrentTemperature(this->temperature);       // instantiate the Current Temperature Characteristic
      temp->setRange(-50,250);                                  // expand the range from the HAP default of 0-100 to -50 to 100 to allow for negative temperatures
      hum = new Characteristic::CurrentRelativeHumidity(50);
      hum->setRange(0, 100);    // expand the range to 30%-100% 
      // Init state
      #define DHTTYPE DHT11
      sensorsDht = new DHT(deviceData[this->deviceid].pin_1, DHTTYPE);
      sensorsDht->begin(); 
      strcpy(deviceData[this->deviceid].state_text, String(this->temperature).c_str());
      strcat(deviceData[this->deviceid].state_text, " C");
      strcat(deviceData[this->deviceid].state_text, " - ");
      strcat(deviceData[this->deviceid].state_text, String(this->humidity).c_str());
      strcat(deviceData[this->deviceid].state_text, " %");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "dht22") == 0) { // Init DHT Pointers
      temp = new Characteristic::CurrentTemperature(this->temperature);       // instantiate the Current Temperature Characteristic
      temp->setRange(-50,250);                                  // expand the range from the HAP default of 0-100 to -50 to 100 to allow for negative temperatures
      hum = new Characteristic::CurrentRelativeHumidity(this->humidity);
      hum->setRange(0, 100);    // expand the range to 30%-100% 
      // Init state
      #define DHTTYPE DHT22
      sensorsDht = new DHT(deviceData[this->deviceid].pin_1, DHTTYPE);
      sensorsDht->begin(); 
      strcpy(deviceData[this->deviceid].state_text, String(this->temperature).c_str());
      strcat(deviceData[this->deviceid].state_text, " C");
      strcat(deviceData[this->deviceid].state_text, " - ");
      strcat(deviceData[this->deviceid].state_text, String(this->humidity).c_str());
      strcat(deviceData[this->deviceid].state_text, " %");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } 

  void loop() {
    // wait
    if (this->initsuccess && temp->timeVal() > this->waitingtime) {
      getTemp();    
    }
  } // loop

  void getTemp() {
    // Get state
    if (strcmp(deviceData[this->deviceid].type.code, "ds18b20") == 0) {
      sensorsDallas->requestTemperatures();
      this->temperature = sensorsDallas->getTempCByIndex(0) + deviceData[this->deviceid].float_1;// Celsius
      //this->temperature = sensorsDallas->getTempFByIndex(0) + deviceData[this->deviceid].offset_temp;// Fahrenheit
      if (this->temperature<=-50) { this->temperature = -50; logDevice(this->deviceid,logKind::ERROR,"error temperature value"); }
      if (isnan(this->temperature)) { this->temperature = 0; logDevice(this->deviceid,logKind::ERROR,"error reading temperature"); }
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "dht11") == 0 || strcmp(deviceData[this->deviceid].type.code, "dht22") == 0) {
      this->temperature = sensorsDht->readTemperature() + deviceData[this->deviceid].float_1;   // Celsius
      //this->temperature = sensorsDht->readTemperature(true) + deviceData[this->deviceid].offset_temp;   // Fahrenheit
      this->humidity = sensorsDht->readHumidity() + deviceData[this->deviceid].float_2; 
      if (this->temperature<=-50) { this->temperature = -50; logDevice(this->deviceid,logKind::ERROR,"error temperature value"); }
      if (this->humidity<=0) { this->humidity = 0; logDevice(this->deviceid,logKind::ERROR,"error humidity value"); }
      if (isnan(this->temperature)) { this->temperature = 0; logDevice(this->deviceid,logKind::ERROR,"error reading temperature"); }
      if (isnan(this->humidity)) { this->humidity = 0; logDevice(this->deviceid,logKind::ERROR,"error reading humidity"); }
    }
    // Set state
    if (strcmp(deviceData[this->deviceid].type.code, "ds18b20") == 0) {
      temp->setVal(this->temperature); 
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "dht11") == 0 || strcmp(deviceData[this->deviceid].type.code, "dht22") == 0) {
      temp->setVal(this->temperature); 
      hum->setVal(this->humidity);
    }
    // Device state
    if (strcmp(deviceData[this->deviceid].type.code, "ds18b20") == 0) {
      strcpy(deviceData[this->deviceid].state_text, String(this->temperature).c_str());
      strcat(deviceData[this->deviceid].state_text, " C");
      deviceData[this->deviceid].state_marked = false;
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "dht11") == 0 || strcmp(deviceData[this->deviceid].type.code, "dht22") == 0) {
      strcpy(deviceData[this->deviceid].state_text, String(this->temperature).c_str());
      strcat(deviceData[this->deviceid].state_text, " C");
      strcat(deviceData[this->deviceid].state_text, " - ");
      strcat(deviceData[this->deviceid].state_text, String(this->humidity).c_str());
      strcat(deviceData[this->deviceid].state_text, " %");
      deviceData[this->deviceid].state_marked = false;
    }
    // Log
    logDevice(this->deviceid,logKind::CHANGES);
  }  //getTemp()
};
// Device: Humidity Sensor (not used and not tested)
struct deviceHumiditySensor : Service::HumiditySensor {      // A Humidity sensor 

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  float humidity = 0;
  // Timer
  float waitingtime = 10000;   

  DHT *sensorsDht;

  SpanCharacteristic *hum;                                     

  deviceHumiditySensor(int sDeviceid) : Service::HumiditySensor() {      

    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "dht11") == 0) { // Init DHT Pointers
      hum = new Characteristic::CurrentRelativeHumidity(50);   // instantiate the Current Temperature Characteristic
      hum->setRange(0, 100);    // expand the range to 30%-100% 
      // Init State
      #define DHTTYPE DHT11
      sensorsDht = new DHT(deviceData[this->deviceid].pin_1, DHTTYPE);
      sensorsDht->begin(); 
      this->humidity = 0;
      strcpy(deviceData[this->deviceid].state_text, String(this->humidity).c_str());
      strcat(deviceData[this->deviceid].state_text, " %");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "dht22") == 0) { // Init DHT Pointers
      hum = new Characteristic::CurrentRelativeHumidity(50);   // instantiate the Current Temperature Characteristic
      hum->setRange(0, 100);    // expand the range to 30%-100% 
      // Init State
      #define DHTTYPE DHT22
      sensorsDht = new DHT(deviceData[this->deviceid].pin_1, DHTTYPE);
      sensorsDht->begin(); 
      this->humidity = 0;
      strcpy(deviceData[this->deviceid].state_text, String(this->humidity).c_str());
      strcat(deviceData[this->deviceid].state_text, " %");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } 

  void loop() {
    // wait
    if (this->initsuccess && hum->timeVal() > this->waitingtime) {
      getHumidity();    
    }
  } // loop
  
  void getHumidity() {
    // Get state
    if (strcmp(deviceData[this->deviceid].type.code, "dht11") == 0 || strcmp(deviceData[this->deviceid].type.code, "dht22") == 0) {
      this->humidity = sensorsDht->readHumidity() + deviceData[this->deviceid].float_1;  
    }
    if (this->humidity<=0) { this->humidity = 0; logDevice(this->deviceid,logKind::ERROR,"error humidity value"); }
    if (isnan(this->humidity)) { this->humidity = 0; logDevice(this->deviceid,logKind::ERROR,"error reading humidity"); }
    // Set state
    hum->setVal(this->humidity); 
    strcpy(deviceData[this->deviceid].state_text, String(this->humidity).c_str());
    strcat(deviceData[this->deviceid].state_text, " %");
    deviceData[this->deviceid].state_marked = false;
    // Log
    logDevice(this->deviceid,logKind::CHANGES); 
  }  //getHumidity()
};
// Device: Light Sensor (ok)
struct deviceLightSensor : Service::LightSensor {      // A Light sensor 

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  float lightlevel = 0;
  // Timer
  float waitingtime = 10000;                                

  BH1750 lightMeter;

  SpanCharacteristic *light;                                     

  deviceLightSensor(int sDeviceid) : Service::LightSensor() {      

    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "bh1750") == 0) { // Init Pointers
      light = new Characteristic::CurrentAmbientLightLevel(1);   // instantiate the Current Light Level Characteristic
      light->setRange(0.001, 10000);    // expand the range 
      // Init State
      lightMeter.begin();  // I2C needed to be intialized before!!!! (done during setup)
      this->lightlevel = 0;
      strcpy(deviceData[this->deviceid].state_text, String(this->lightlevel).c_str());
      strcat(deviceData[this->deviceid].state_text, " lx");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "temt6000") == 0) { // Init Pointers
      light = new Characteristic::CurrentAmbientLightLevel(1);   // instantiate the Current Light Level Characteristic
      light->setRange(0.001, 10000);    // expand the range 
      // Init State
      pinMode(deviceData[this->deviceid].pin_1, INPUT); // ADC1 pin needed
      this->lightlevel = 0;
      strcpy(deviceData[this->deviceid].state_text, String(this->lightlevel).c_str());
      strcat(deviceData[this->deviceid].state_text, " lx");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } // end constructo

  void loop() {
    // wait
    if (this->initsuccess && light->timeVal() > this->waitingtime) {
      getLightLevel();    
    }
  } // loop

  void getLightLevel() {
    // Get state
    if (strcmp(deviceData[this->deviceid].type.code, "bh1750") == 0) {
      this->lightlevel = lightMeter.readLightLevel() + deviceData[this->deviceid].float_1;  
    }
    else if (strcmp(deviceData[this->deviceid].type.code, "temt6000") == 0) {
      analogReadResolution(10);
      float volts =  analogRead(deviceData[this->deviceid].pin_1) * 5 / 1024.0; // Convert reading to VOLTS
      float amps = volts / 10000.0;  // em 10,000 Ohms
      float microamps = amps * 1000000; // Convert to Microamps
      float lux = microamps * 2.0; // Convert to Lux */
      this->lightlevel = lux + deviceData[this->deviceid].float_1;  
    }
    if (this->lightlevel<=0.001) { this->lightlevel = 0.001; logDevice(this->deviceid,logKind::ERROR,"error lightlevel value"); }
    if (isnan(this->lightlevel)) { this->lightlevel = 0.001; logDevice(this->deviceid,logKind::ERROR,"error reading lightlevel"); }
    // Set state
    light->setVal(this->lightlevel);     
    // Device State
    strcpy(deviceData[this->deviceid].state_text, String(this->lightlevel).c_str());
    strcat(deviceData[this->deviceid].state_text, " lx");
    deviceData[this->deviceid].state_marked = false; 
    // Log
    logDevice(this->deviceid,logKind::CHANGES); 
  }  //getLightLevel()
};
// ############ BIT SENSORS
// Device: Leak Sensor (no leak sensor implemented)
struct deviceLeakSensor : Service::LeakSensor {     // A Leak Sensor

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  bool isleak = false;
  bool isleak_old = false;
  // Timer
  float waitingtime = 100;  

  SpanCharacteristic *leak;            

  deviceLeakSensor(int sDeviceid) : Service::LeakSensor(){       // constructor() method
        
    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "leak") == 0) { 
      leak=new Characteristic::LeakDetected(1);              // initial value of 1 = open - 0 = closed
      // Init State
      pinMode(deviceData[this->deviceid].pin_1,INPUT);
      this->isleak = false;
      strcpy(deviceData[this->deviceid].state_text, "no leak"); 
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT); 
      this->initsuccess = true;
    }
  } // end constructor

  void loop() {
    // the temperature refreshes every 100 millsec by the elapsed time
    if (this->initsuccess && leak->timeVal() > this->waitingtime) {
      this->waitingtime = 100; // standard waiting time
      getLeak();    
    }
  } // loop

  void getLeak() {
    if (strcmp(deviceData[this->deviceid].type.code, "leak") == 0) { 
      if (digitalRead(deviceData[this->deviceid].pin_1) == 1) this->isleak = true; 
      else this->isleak = false;
    }
    // If new state diff
    if (this->isleak_old != this->isleak) {
      leak->setVal(this->isleak); // Set new state
      this->isleak_old = this->isleak;
      // Set Timer
      if (this->isleak) this->waitingtime = deviceData[this->deviceid].float_1; // Extended waiting time after motion detected
      // Device State
      if (this->isleak) { strcpy(deviceData[this->deviceid].state_text, "leak detected"); deviceData[this->deviceid].state_marked = true; }
      else { strcpy(deviceData[this->deviceid].state_text, "no leak"); deviceData[this->deviceid].state_marked = false; }
      // Log
      logDevice(this->deviceid,logKind::CHANGES);
    }
  }  //getMotion()
};
// Device: Motion Sensor (ok) noch prüfen ob der status in homekit richtig herum ist
struct deviceMotionSensor : Service::MotionSensor {     // A Motion Sensor

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  bool inmotion = false;
  bool inmotion_old = false;
  // Timer
  float waitingtime = 100; 

  SpanCharacteristic *motion;            

  deviceMotionSensor(int sDeviceid) : Service::MotionSensor(){       // constructor() method
    
    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "sw420") == 0 || strcmp(deviceData[this->deviceid].type.code, "hcsr501") == 0) { 
      motion=new Characteristic::MotionDetected(1);              // initial value of 1 = open - 0 = closed
      // Init State
      pinMode(deviceData[this->deviceid].pin_1,INPUT);
      this->inmotion = false;
      strcpy(deviceData[this->deviceid].state_text, "no motion"); 
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } // end constructo

  void loop() {
    // the temperature refreshes every 100 millsec by the elapsed time
    if (this->initsuccess && motion->timeVal() > this->waitingtime) {
      this->waitingtime = 100; // standard waiting time
      getMotion();    
    }
  } // loop

  void getMotion() {
    if (strcmp(deviceData[this->deviceid].type.code, "sw420") == 0 || strcmp(deviceData[this->deviceid].type.code, "hcsr501") == 0) { 
      if (digitalRead(deviceData[this->deviceid].pin_1) == 1) this->inmotion = true; 
      else this->inmotion = false;
    }
    // If new state diff
    if (this->inmotion_old != this->inmotion) {
      motion->setVal(this->inmotion); // Set new state
      this->inmotion_old = this->inmotion;
      // Set Timer
      if (this->inmotion) this->waitingtime = deviceData[this->deviceid].float_1; // Extended waiting time after motion detected
      // Device State
      if (this->inmotion) { strcpy(deviceData[this->deviceid].state_text, "in motion"); deviceData[this->deviceid].state_marked = true; }
      else { strcpy(deviceData[this->deviceid].state_text, "no motion"); deviceData[this->deviceid].state_marked = false; }
      // Log
      logDevice(this->deviceid,logKind::CHANGES);
    }
  }  //getMotion()
};
// Device: Smoke Sensor (ok) noch prüfen ob der status in homekit richtig herum ist
struct deviceSmokeSensor : Service::SmokeSensor {     // A Smoke Sensor

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  bool issmoke;
  bool issmoke_old;
  // Timer
  float waitingtime = 100; 

  SpanCharacteristic *smoke;            

  deviceSmokeSensor(int sDeviceid) : Service::SmokeSensor(){       // constructor() method
        
    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "mq2") == 0) { 
      smoke=new Characteristic::SmokeDetected(1);              // initial value of 1 = open - 0 = closed
      // Init State
      pinMode(deviceData[this->deviceid].pin_1,INPUT);
      this->issmoke = false;
      strcpy(deviceData[this->deviceid].state_text, "no smoke"); 
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } // end constructo

  void loop() {
    // the temperature refreshes every 100 millsec by the elapsed time
    if (this->initsuccess && smoke->timeVal() > this->waitingtime) {
      this->waitingtime = 100; // standard waiting time
      getSmoke();    
    }
  } // loop

  void getSmoke() {
    if (strcmp(deviceData[this->deviceid].type.code, "mq2") == 0) {
      if (digitalRead(deviceData[this->deviceid].pin_1) == 1) this->issmoke = true;
      else this->issmoke = false;
    }
    // If new state diff
    if (this->issmoke_old != this->issmoke) {
      smoke->setVal(this->issmoke); // Set new state
      this->issmoke_old = this->issmoke;
      // Set Timer
      if (this->issmoke) this->waitingtime = deviceData[this->deviceid].float_1; // Extended waiting time after motion detected
      // Device State
      if (this->issmoke) { strcpy(deviceData[this->deviceid].state_text, "smoke detected"); deviceData[this->deviceid].state_marked = true; }
      else { strcpy(deviceData[this->deviceid].state_text, "no smoke"); deviceData[this->deviceid].state_marked = false; }
      // Log
      logDevice(this->deviceid,logKind::CHANGES); 
    }
  }  //getSmokes()
};

// ############ IN PROCESS
// Device: Battery Service (not implemented)
struct deviceBatteryService : Service::BatteryService {      // A Battery Service

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  float batterylevel = 100;
  bool ischarging = false;
  bool islow = false;
  // Timer
  float waitingtime = 10000;                                

  SpanCharacteristic *level;  //BatteryLevel uint8_t [0,100]
  SpanCharacteristic *charging; //ChargingState uint8_t [0,2]
  SpanCharacteristic *low;    //StatusLowBattery uint8_t [0,1]             

  deviceBatteryService(int sDeviceid) : Service::BatteryService() {      

    this->deviceid = sDeviceid;

    if (strcmp(deviceData[this->deviceid].type.code, "batmodbus") == 0) { // Init Pointers
      level = new Characteristic::BatteryLevel(1);   // instantiate the Current Light Level Characteristic
      charging = new Characteristic::ChargingState(1);   // instantiate the Current Light Level Characteristic
      low = new Characteristic::StatusLowBattery(1);   // instantiate the Current Light Level Characteristic
      // Init State
      this->batterylevel = 100;
      this->ischarging = false;
      this->islow = false;
      strcpy(deviceData[this->deviceid].state_text, String(this->batterylevel).c_str());
      strcat(deviceData[this->deviceid].state_text, "%");
      if (this->ischarging) strcat(deviceData[this->deviceid].state_text, " charge");
      if (this->islow) strcat(deviceData[this->deviceid].state_text, " low");
      deviceData[this->deviceid].state_marked = false;
      // Init Log
      logDevice(this->deviceid,logKind::INIT);
      this->initsuccess = true;
    }
  } // end constructo

  void loop() {
    // wait
    if (this->initsuccess && level->timeVal() > this->waitingtime) {
      getBatteryInfo();    
    }
  } // loop

  void getBatteryInfo() {
    // Set state
    level->setVal(this->batterylevel);   
    charging->setVal(this->ischarging);
    low->setVal(this->islow);  
    // Device State
    strcpy(deviceData[this->deviceid].state_text, String(this->batterylevel).c_str());
    strcat(deviceData[this->deviceid].state_text, "%");
    if (this->ischarging) strcat(deviceData[this->deviceid].state_text, " charge");
    if (this->islow) strcat(deviceData[this->deviceid].state_text, " low");
    if (this->ischarging) deviceData[this->deviceid].state_marked = true; 
    else deviceData[this->deviceid].state_marked = false; 
    // Log
    logDevice(this->deviceid,logKind::CHANGES);
  }  //getBatteryInfo()
};
// Device: Outlet (not implemented)
struct deviceOutlet : Service::Outlet {       // Outlet

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  bool ison = false;
  
  SpanCharacteristic *power;                        // reference to the On Characteristic
 
  deviceOutlet(int sDeviceid) : Service::Outlet(){

    this->deviceid = sDeviceid;

    power=new Characteristic::On(0,true);           // second argument is true, so the value of the On Characteristic (initially set to 0) will be saved in NVS


    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  boolean update(){                             
    // Get State
    if(power->updated()) {
      if (power->getNewVal()) strcpy(deviceData[this->deviceid].state_text, "on");
      else strcpy(deviceData[this->deviceid].state_text, "on");
    }

    logDevice(this->deviceid,logKind::CHANGES); 
   
    return(true);                               // return true
  } // update

};
// Device: Garage Door Opener (not implemented)
struct deviceGarageDoorOpener : Service::GarageDoorOpener { // A Garage Door Opener

  // Settings
  bool initsuccess = false;
  int deviceid = 0;
  // State
  int state = 0;
  // Timer
  float waitingtime = 10000; 

  SpanCharacteristic *current; // reference to the Current Door State Characteristic (specific to Garage Door Openers)
  SpanCharacteristic *target; // reference to the Target Door State Characteristic (specific to Garage Door Openers)
  SpanCharacteristic *obstruction; // reference to the Obstruction Detected Characteristic (specific to Garage Door Openers)

  deviceGarageDoorOpener(int sDeviceid) : Service::GarageDoorOpener(){ // constructor() method

    this->deviceid = sDeviceid;

    current = new Characteristic::CurrentDoorState(1);              // initial value of 1 means closed
    target = new Characteristic::TargetDoorState(1);                // initial value of 1 means closed
    obstruction = new Characteristic::ObstructionDetected(false);   // initial value of false means NO obstruction is detected

    pinMode(deviceData[this->deviceid].pin_1, OUTPUT);
    pinMode(deviceData[this->deviceid].pin_2, OUTPUT);

    Serial.print("Configuring Garage Door Opener");   // initialization message
    Serial.print("\n");

    // Init Log
    logDevice(this->deviceid,logKind::INIT);
    this->initsuccess = true;
  } // end constructor

  boolean update(){ // update() method

    int targetVal = target->getNewVal();
    int currentVal = current->getNewVal();

    if(targetVal == 0){
      LOG1("Opening Garage Door\n");
      digitalWrite(deviceData[this->deviceid].pin_1, current->getNewVal());
      digitalWrite(deviceData[this->deviceid].pin_2, !current->getNewVal());

      obstruction->setVal(false);

    } else if(targetVal == 1){
      LOG1("Closing Garage Door\n");

      digitalWrite(deviceData[this->deviceid].pin_1, !current->getNewVal());
      digitalWrite(deviceData[this->deviceid].pin_2, current->getNewVal());

      obstruction->setVal(false);

    }
    return(true); // return true

  } // update

  void loop(){ // loop() method

    if(current->getVal()==target->getVal())        // if current-state matches target-state there is nothing do -- exit loop()
      return;

    if(current->getVal()==3 && random(100000)==0){    // here we simulate a random obstruction, but only if the door is closing (not opening)
      digitalWrite(deviceData[this->deviceid].pin_1, LOW);
      digitalWrite(deviceData[this->deviceid].pin_2, LOW);
      current->setVal(4);                             // if our simulated obstruction is triggered, set the curent-state to 4, which means "stopped"
      obstruction->setVal(true);                      // and set obstruction-detected to true
      LOG1("Garage Door Obstruction Detected!\n");
    }

    if(current->getVal()==4)                       // if the current-state is stopped, there is nothing more to do - exit loop()     
      return;

    // This last bit of code only gets called if the door is in a state that represents actively opening or actively closing.
    // If there is an obstruction, the door is "stopped" and won't start again until the HomeKit Controller requests a new open or close action

    if(target->timeVal()>5000)                     // simulate a garage door that takes 5 seconds to operate by monitoring time since target-state was last modified
      current->setVal(target->getVal());           // set the current-state to the target-state

  } // loop

};

