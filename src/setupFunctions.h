void setupESP32(){
  Serial.begin(115200); 
  //pinMode(onboard_led.pin, OUTPUT);
  //pinMode(button.pin,      INPUT);
}

void setupOledScreen(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE,BLACK);  
  display.setCursor (0,0);  
}
void setupSPIFFS(){
  if( !SPIFFS.begin()){
    Serial.println("Error mounting SPIFFS");
    while(1);
  }  
}
void setupEEPROM(){
  const int EEPROM_size = 512;
  EEPROM.begin(EEPROM_size);
  DEBUG_PRINT(F("dedicated EEPROM memory capacity: ")); DEBUG_PRINTLN(EEPROM_size);
  DEBUG_PRINT(F("EEPROM memory in use: ")); DEBUG_PRINTLN(sizeof(myVar));
  
  #ifdef ESP8266
  int check;
  EEPROM.get(0, check);
  #else
  int check  = EEPROM.readInt(0);
  #endif
  
  if (check == 10001){
  display.print(F("code: ")); display.println(check);
  DEBUG_PRINT(F("check number is: ")); DEBUG_PRINTLN(check);
  //display.println(F("EEPROM SET"));
  display.display();
  initializeEEPROMvariables(); // functio in global_variables.h at the bottom.    
  delay(1000);
  }
  
  else if (check != 10001){
  display.println(F("EEPROM not initialized"));
  DEBUG_PRINTLN(F("EEPROM not initialized"));
  display.println(F("Write to EEPROM"));
  display.display();
  int eeAddress=0;
  EEPROM.put(eeAddress, myVar);
  EEPROM.commit();
  display.println(F("EEPROM is initialized"));
  display.display();
  delay(2000);
  ESP.restart();
  }
  display.clearDisplay();  
}

void setupRTC(){
  if (systemParam.externalRTC){
    if (!rtc.begin()) {
      DEBUG_PRINTLN(F("Couldn't find RTC"));
      systemParam.externalRTC = false;
      EEPROM.put(offsetof(storeEEPROM, systemParam.externalRTC), (systemParam.externalRTC));
      EEPROM.commit();
      //while (1);
    }
  }
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void setupWIFI(){
  int wifiCounter=0;
  WiFi.mode(WIFI_STA);
  const char *WIFI_SSID = &wifiID[0];
  const char *WIFI_PASS = &wifiPASS[0];
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      display.clearDisplay();
      display.setCursor(0,0);
      display.println(F("connecting to wifi..."));
      display.display();
      delay(500);
      wifiCounter++;
      wifiStationMode = true;

      if (wifiCounter > 20){
      wifiStationMode = false;  
      break;  
      }
  }
  if (wifiCounter >20){
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID_AP, WIFI_PASS_AP);
  Serial.println(F("soft-AP IP:"));
  display.println(F("soft-AP IP:"));
  Serial.println(WiFi.softAPIP());
  display.println(WiFi.softAPIP());
  display.display();    
  }
  if (wifiStationMode){
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
  }
  else{
  Serial.printf(" %s\n", WiFi.softAPIP().toString().c_str());
  }
  
  display.clearDisplay();
  display.setCursor(0,0);

  if (wifiStationMode){
  display.printf(" %s\n", WiFi.localIP().toString().c_str());
  }
  else {
  display.printf(" %s\n", WiFi.softAPIP().toString().c_str());
  }
    
  initWebSocket();
  initWebServer();
    
  display.println(F("setup complete"));
//    display.println(F("mDNS: esp32.local/"));
  display.display();  
  
  delay(1000);   
}

void setupTempSensors(){
  if (systemParam.probeTypeT == 1){
      for (int sensor = 0; sensor < systemParam.probeCountT; sensor++){
      maxthermo[sensor].begin(MAX31865_2WIRE);  // set to 3WIRE or 4WIRE as necessary
      //maxthermo[sensor].begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
      //maxthermo[sensor].begin(MAX31865_4WIRE);  // set to 2WIRE or 3WIRE as necessary
      }
  }
  else if (systemParam.probeTypeT == 2) {
    for (int sensor = 0; sensor < systemParam.probeCountT; sensor++){    
    thermocouple[0] = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN); // sensor 1
    thermocouple[1] = new MAX6675_Thermocouple(SCK_PIN, CS2_PIN, SO_PIN); // sensor 2
    // requiers more inputs
  }
  }
}

void setupDHTSensors(){
  if (systemParam.probeCountH > 0){
    for (int sensor = 0; sensor < systemParam.probeCountH; sensor++){
      dht[sensor].begin();
    }
  }
  else {
  return;
  }
}

void setupFans(){
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  ledcAttachPin(OUTPUT_PIN1, ledChannel1);
  ledcAttachPin(OUTPUT_PIN2, ledChannel2);
  if (!fan1.connected){
      ledcWrite(ledChannel1, 0);
  }
  if (!fan2.connected){
        ledcWrite(ledChannel2, 0);
  }
}

void setupRelays(){
  pinMode(RELAYPIN1,   OUTPUT);
  digitalWrite(RELAYPIN1, true);
  
  pinMode(RELAYPIN2,   OUTPUT);
  digitalWrite(RELAYPIN2, true);
  
  pinMode(RELAYPIN3,   OUTPUT);
  digitalWrite(RELAYPIN3, true);
  
  pinMode(RELAYPIN4,   OUTPUT);
  digitalWrite(RELAYPIN4, true);
  
  pinMode(RELAYPIN5,   OUTPUT);
  digitalWrite(RELAYPIN5, true);
  
  pinMode(RELAYPIN6 ,   OUTPUT);
 digitalWrite(RELAYPIN6, true);
}

void initializeEEPROMvariables(){

//systemParam.probeTypeT  = EEPROM.read(offsetof(storeEEPROM, systemParam.probeTypeT));

systemParam.probeTypeT  = EEPROM.read(offsetof(storeEEPROM, systemParam.probeTypeT));
systemParam.probeCountT = EEPROM.read(offsetof(storeEEPROM, systemParam.probeCountT));
systemParam.probeTypeH  = EEPROM.read(offsetof(storeEEPROM, systemParam.probeTypeH));
systemParam.probeCountH = EEPROM.read(offsetof(storeEEPROM, systemParam.probeCountH));
systemParam.measurements = EEPROM.read(offsetof(storeEEPROM, systemParam.measurements));
systemParam.externalRTC = EEPROM.read(offsetof(storeEEPROM, systemParam.externalRTC));
//systemParam.resetRTC = EEPROM.read(offsetof(storeEEPROM, systemParam.resetRTC));

relay1.connected = EEPROM.read(offsetof(storeEEPROM, relay1.connected));
relay2.connected = EEPROM.read(offsetof(storeEEPROM, relay2.connected));
relay3.connected = EEPROM.read(offsetof(storeEEPROM, relay3.connected));
relay4.connected = EEPROM.read(offsetof(storeEEPROM, relay4.connected));
relay5.connected = EEPROM.read(offsetof(storeEEPROM, relay5.connected));
relay6.connected = EEPROM.read(offsetof(storeEEPROM, relay6.connected));
relay1.function = EEPROM.read(offsetof(storeEEPROM, relay1.function));
relay2.function = EEPROM.read(offsetof(storeEEPROM, relay2.function));
relay3.function = EEPROM.read(offsetof(storeEEPROM, relay3.function));
relay4.function = EEPROM.read(offsetof(storeEEPROM, relay4.function));
relay5.function = EEPROM.read(offsetof(storeEEPROM, relay5.function));
relay6.function = EEPROM.read(offsetof(storeEEPROM, relay6.function));

fan1.connected = EEPROM.read(offsetof(storeEEPROM, fan1.connected));
fan2.connected = EEPROM.read(offsetof(storeEEPROM, fan2.connected));
fan1.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan1.manualSpeed));
fan2.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan2.manualSpeed));
fan1.manual = EEPROM.read(offsetof(storeEEPROM, fan1.manual));
fan2.manual = EEPROM.read(offsetof(storeEEPROM, fan2.manual));
fan1.daySwitch = EEPROM.read(offsetof(storeEEPROM, fan1.daySwitch));
fan2.daySwitch = EEPROM.read(offsetof(storeEEPROM, fan2.daySwitch));
fan1.output_min = EEPROM.read(offsetof(storeEEPROM, fan1.output_min));
fan1.output_max = EEPROM.read(offsetof(storeEEPROM, fan1.output_max));
fan2.output_min = EEPROM.read(offsetof(storeEEPROM, fan2.output_min));
fan2.output_max = EEPROM.read(offsetof(storeEEPROM, fan2.output_max));

humidParam1.humidmin = EEPROM.read(offsetof(storeEEPROM, humidParam1.humidmin));
humidParam1.humidmax =  EEPROM.read(offsetof(storeEEPROM, humidParam1.humidmax));
humidParam2.humidmin = EEPROM.read(offsetof(storeEEPROM, humidParam2.humidmin));
humidParam2.humidmax =  EEPROM.read(offsetof(storeEEPROM, humidParam2.humidmax));
// humidMin3 = EEPROM.read(offsetof(storeEEPROM, humidMin3));
// humidMax3 =  EEPROM.read(offsetof(storeEEPROM, humidMax3));


#ifdef ESP8266  

EEPROM.get(offsetof(storeEEPROM, systemParam.graphUpdate), systemParam.graphUpdate;
EEPROM.get(offsetof(storeEEPROM, systemParam.tempUodate), systemParam.tempUpdate;

// EEPROM.get(offsetof(storeEEPROM, KP, KP);
// EEPROM.get(offsetof(storeEEPROM, KI), KI);
// EEPROM.get(offsetof(storeEEPROM, KD), KD);

// EEPROM.get(offsetof(storeEEPROM, OUTPUT_MIN1), OUTPUT_MIN1);
// EEPROM.get(offsetof(storeEEPROM, OUTPUT_MAX1), OUTPUT_MAX1);
// EEPROM.get(offsetof(storeEEPROM, OUTPUT_MIN2), OUTPUT_MIN2);
// EEPROM.get(offsetof(storeEEPROM, OUTPUT_MAX2), OUTPUT_MAX2);

EEPROM.get(offsetof(storeEEPROM, daySoilTemp1), daySoilTemp1;
EEPROM.get(offsetof(storeEEPROM, nightSoilTemp1), nightSoilTemp1;
EEPROM.get(offsetof(storeEEPROM, targetAirTemp1), targetAirTemp1;
EEPROM.get(offsetof(storeEEPROM, tempRange1), tempRange1;
EEPROM.get(offsetof(storeEEPROM, alarmRange1), alarmRange1;

EEPROM.get(offsetof(storeEEPROM, daySoilTemp2), daySoilTemp2;
EEPROM.get(offsetof(storeEEPROM, nightSoilTemp2), nightSoilTemp2;
EEPROM.get(offsetof(storeEEPROM, targetAirTemp2, targetAirTemp2;
EEPROM.get(offsetof(storeEEPROM, tempRange2), tempRange2;
EEPROM.get(offsetof(storeEEPROM, alarmRange2), alarmRange2;

EEPROM.get(offsetof(storeEEPROM, daySoilTemp3), daySoilTemp3;
EEPROM.get(offsetof(storeEEPROM, nightSoilTemp3), nightSoilTemp3;
EEPROM.get(offsetof(storeEEPROM, tempRange3), tempRange3;
EEPROM.get(offsetof(storeEEPROM, alarmRange3), alarmRange3;

EEPROM.get(offsetof(storeEEPROM, daySoilTemp4), daySoilTemp4;
EEPROM.get(offsetof(storeEEPROM, nightSoilTemp4), nightSoilTemp4;
EEPROM.get(offsetof(storeEEPROM, tempRange4), tempRange4;
EEPROM.get(offsetof(storeEEPROM, alarmRange4), alarmRange4;

for (int m = 0; m < 8; m++){
int offsetPosition = offsetof(storeEEPROM, calibrationValue[0]);
EEPROM.get(offsetPosition+(4*m), calibrationValue[m]);
}

#else
systemParam.graphUpdate = EEPROM.readInt(offsetof(storeEEPROM, systemParam.graphUpdate));
systemParam.tempUpdate = EEPROM.readInt(offsetof(storeEEPROM, systemParam.tempUpdate));

tempParam1.daySoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam1.daySoilTemp));
tempParam1.nightSoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam1.nightSoilTemp));
tempParam1.targetAirTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam1.targetAirTemp));
tempParam1.tempRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam1.tempRange));
tempParam1.alarmRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam1.alarmRange));

tempParam2.daySoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam2.daySoilTemp));
tempParam2.nightSoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam2.nightSoilTemp));
tempParam2.targetAirTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam2.targetAirTemp));
tempParam2.tempRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam2.tempRange));
tempParam2.alarmRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam2.alarmRange));

tempParam3.daySoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam3.daySoilTemp));
tempParam3.nightSoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam3.nightSoilTemp));
tempParam3.tempRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam3.tempRange));
tempParam3.alarmRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam3.alarmRange));

tempParam4.daySoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam4.daySoilTemp));
tempParam4.nightSoilTemp = EEPROM.readFloat(offsetof(storeEEPROM, tempParam4.nightSoilTemp));
tempParam4.tempRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam4.tempRange));
tempParam4.alarmRange = EEPROM.readFloat(offsetof(storeEEPROM, tempParam4.alarmRange));

// KP = EEPROM.readDouble(offsetof(storeEEPROM, KP));
// KI = EEPROM.readDouble(offsetof(storeEEPROM, KI));
// KD = EEPROM.readDouble(offsetof(storeEEPROM, KD));

// OUTPUT_MIN1 = EEPROM.readDouble(offsetof(storeEEPROM, OUTPUT_MIN1));
// OUTPUT_MAX1 = EEPROM.readDouble(offsetof(storeEEPROM, OUTPUT_MAX1));
// OUTPUT_MIN2 = EEPROM.readDouble(offsetof(storeEEPROM, OUTPUT_MIN2));
// OUTPUT_MAX2 = EEPROM.readDouble(offsetof(storeEEPROM, OUTPUT_MAX2));

cal.pt100[0] = EEPROM.readFloat(offsetof(storeEEPROM, cal.pt100[0]));
cal.pt100[1] = EEPROM.readFloat(offsetof(storeEEPROM, cal.pt100[1]));
cal.pt100[2] = EEPROM.readFloat(offsetof(storeEEPROM, cal.pt100[2]));
cal.pt100[3] = EEPROM.readFloat(offsetof(storeEEPROM, cal.pt100[3]));
cal.dhtT[0] = EEPROM.readFloat(offsetof(storeEEPROM, cal.dhtT[0]));
cal.dhtT[1] = EEPROM.readFloat(offsetof(storeEEPROM, cal.dhtT[1]));
cal.dhtH[0] = EEPROM.readFloat(offsetof(storeEEPROM, cal.dhtH[0]));
cal.dhtH[1] = EEPROM.readFloat(offsetof(storeEEPROM, cal.dhtH[1]));


// for (int m = 0; m < 8; m++){
// int offsetPosition = offsetof(storeEEPROM, cal.pt100[0]);
// calibrationValue[m]  = EEPROM.readFloat(offsetPosition+4*m);
// }
#endif

tempParam1.targetSoilTemp = tempParam1.daySoilTemp;
tempParam2.targetSoilTemp = tempParam2.daySoilTemp;
tempParam3.targetSoilTemp = tempParam3.daySoilTemp;
tempParam4.targetSoilTemp = tempParam4.daySoilTemp;
// for (int m = 0; m < 8; m++){
// int offsetPosition = offsetof(storeEEPROM, lightSchedule[0]);
// lightSchedule[m]  = EEPROM.readChar(offsetPosition+m);
// // Serial.println(lightSchedule[m]);
// }

//Serial.print("position: "); Serial.print(offsetPosition+4*m); Serial.print(" cal. value: "); Serial.println(calibrationValue[m]);

for (int m = 0; m < 32; m++){
int offsetPosition = offsetof(storeEEPROM, systemParam.SSID[0]);
systemParam.SSID[m]  = EEPROM.read(offsetPosition+m);
}
wifiID = String(systemParam.SSID);
DEBUG_PRINT(F("wifiID ")); DEBUG_PRINTLN(wifiID);

for (int m = 0; m < 32; m++){  
int offsetPosition = offsetof(storeEEPROM, systemParam.PASS[0]);
systemParam.PASS[m]  = EEPROM.read(offsetPosition+m);
}
wifiPASS = String(systemParam.PASS);
DEBUG_PRINT(F("wifiPASS ")); DEBUG_PRINTLN(wifiPASS);


for (int m = 0; m < 6; m++){
int offsetPosition = offsetof(storeEEPROM, lights1.lightsOn[0]);
schedule1.lights.lightsOn[m]  = EEPROM.read(offsetPosition+m);
}
schedule1.lightsON = String(schedule1.lights.lightsOn);

for (int m = 0; m < 6; m++){
int offsetPosition = offsetof(storeEEPROM, lights1.lightsOff[0]);
schedule1.lights.lightsOff[m]  = EEPROM.read(offsetPosition+m);
}
schedule1.lightsOFF = String(schedule1.lights.lightsOff);



for (int m = 0; m < 6; m++){
int offsetPosition = offsetof(storeEEPROM, lights2.lightsOn[0]);
schedule2.lights.lightsOn[m]  = EEPROM.read(offsetPosition+m);
}
schedule2.lightsON = String(schedule2.lights.lightsOn);

for (int m = 0; m < 6; m++){
int offsetPosition = offsetof(storeEEPROM, lights2.lightsOff[0]);
schedule2.lights.lightsOff[m]  = EEPROM.read(offsetPosition+m);
}
schedule2.lightsOFF = String(schedule2.lights.lightsOff);



for (int m = 0; m < 6; m++){
int offsetPosition = offsetof(storeEEPROM, lights3.lightsOn[0]);
schedule3.lights.lightsOn[m]  = EEPROM.read(offsetPosition+m);
}
schedule3.lightsON = String(schedule3.lights.lightsOn);
//Serial.print("lights2on "); Serial.println(lights2ON);

for (int m = 0; m < 6; m++){
int offsetPosition = offsetof(storeEEPROM, lights3.lightsOff[0]);
schedule3.lights.lightsOff[m]  = EEPROM.read(offsetPosition+m);
}
schedule3.lightsOFF = String(schedule3.lights.lightsOff);    

reInitializeTimeInts();

}


int convertStringToInt(String str, int workflow){
  int str_len = str.length() + 1;
  char char_array[str_len];
  str.toCharArray(char_array, str_len);
  
  char buffer1[3];

  switch (workflow){
    case 0:
      buffer1[0]=char_array[0];
      buffer1[1]=char_array[1];
      buffer1[2]='\0';
    break;
    case 1:
      buffer1[0]=char_array[3];
      buffer1[1]=char_array[4];
      buffer1[2]='\0';
    break;
  }
  return atoi(buffer1);
}


void reInitializeTimeStrings(){
    for (int m = 0; m < 6; m++){
    int offsetPosition = offsetof(storeEEPROM, lights1.lightsOn[0]);
    schedule1.lights.lightsOn[m]  = EEPROM.read(offsetPosition+m);
    }
    schedule1.lightsON = String(schedule1.lights.lightsOn);

    for (int m = 0; m < 6; m++){
    int offsetPosition = offsetof(storeEEPROM, lights1.lightsOff[0]);
    schedule1.lights.lightsOff[m]  = EEPROM.read(offsetPosition+m);
    }
    schedule1.lightsOFF = String(schedule1.lights.lightsOff);



    for (int m = 0; m < 6; m++){
    int offsetPosition = offsetof(storeEEPROM, lights2.lightsOn[0]);
    schedule2.lights.lightsOn[m]  = EEPROM.read(offsetPosition+m);
    }
    schedule2.lightsON = String(schedule2.lights.lightsOn);

    for (int m = 0; m < 6; m++){
    int offsetPosition = offsetof(storeEEPROM, lights2.lightsOff[0]);
    schedule2.lights.lightsOff[m]  = EEPROM.read(offsetPosition+m);
    }
    schedule2.lightsOFF = String(schedule2.lights.lightsOff);



    for (int m = 0; m < 6; m++){
    int offsetPosition = offsetof(storeEEPROM, lights3.lightsOn[0]);
    schedule3.lights.lightsOn[m]  = EEPROM.read(offsetPosition+m);
    }
    schedule3.lightsON = String(schedule3.lights.lightsOn);
    //Serial.print("lights2on "); Serial.println(lights2ON);

    for (int m = 0; m < 6; m++){
    int offsetPosition = offsetof(storeEEPROM, lights3.lightsOff[0]);
    schedule3.lights.lightsOff[m]  = EEPROM.read(offsetPosition+m);
    }
    schedule3.lightsOFF = String(schedule3.lights.lightsOff);    

    reInitializeTimeInts();

    return;
}

void reInitializeTimeInts(){
    schedule1.hoursOn = convertStringToInt(schedule1.lightsON, 0);
    schedule1.minutesOn = convertStringToInt(schedule1.lightsON, 1);
    schedule1.hoursOff = convertStringToInt(schedule1.lightsOFF, 0);
    schedule1.minutesOff = convertStringToInt(schedule1.lightsOFF, 1);
    currentMinutes = ((hours*60)+minutes);

    schedule1.minutesLightsOn = ((schedule1.hoursOn*60)+schedule1.minutesOn);
    schedule1.minutesLightsOff = ((schedule1.hoursOff*60)+schedule1.minutesOff);

    schedule2.hoursOn = convertStringToInt(schedule2.lightsON, 0);
    schedule2.minutesOn = convertStringToInt(schedule2.lightsON, 1);
    schedule2.hoursOff = convertStringToInt(schedule2.lightsOFF, 0);
    schedule2.minutesOff = convertStringToInt(schedule2.lightsOFF, 1);

    schedule2.minutesLightsOn = ((schedule2.hoursOn*60)+schedule2.minutesOn);
    schedule2.minutesLightsOff = ((schedule2.hoursOff*60)+schedule2.minutesOff);

    schedule3.hoursOn = convertStringToInt(schedule3.lightsON, 0);
    schedule3.minutesOn = convertStringToInt(schedule3.lightsON, 1);
    schedule3.hoursOff = convertStringToInt(schedule3.lightsOFF, 0);
    schedule3.minutesOff = convertStringToInt(schedule3.lightsOFF, 1);
 
    schedule3.minutesLightsOn = ((schedule3.hoursOn*60)+schedule3.minutesOn);
    schedule3.minutesLightsOff = ((schedule3.hoursOff*60)+schedule3.minutesOff);    
}

void syncTimeRTC(){ // This function syncs RTC timer upon connection to wifi in case lostPower is true.
if (systemParam.externalRTC){
      if ((rtc.lostPower())  || (systemParam.resetRTC)) {
        Serial.println("RTC lost power, lets set the time!");

        configTime(3600, 3600, ntpServer);
        // following line sets the RTC to the date &amp; time this sketch was compiled
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

        struct tm time;
  
        if(!getLocalTime(&time)){
          Serial.println("Could not obtain time info");
          return;
        }
      
        Serial.println("\n---------TIME----------");
        
        Serial.println(asctime(&time));

        rtc.adjust(DateTime(time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec));
        systemParam.resetRTC = false;
        notifyClientsSingleObject("resetRTC", systemParam.resetRTC);
//        EEPROM.put(offsetof(storeEEPROM, systemParam.resetRTC), (systemParam.resetRTC));
//        EEPROM.commit();
      }
  }
}