// String processor(const String &var) {
//     return String(var == "STATE" && relay1.on ? "on" : "off");
// }

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");    // , false, processor
  //request->send(SPIFFS, "/index.js", "text/javascript");
}

void initWebServer() {
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}
             
void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void notifyClientsSingleString(String object, String &message) {
        // String objectString = object;
    //const uint8_t size = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<50> doc;
    String key = object;
    String Value = message;
    doc[key] = Value; 

    //serializeJson(doc, Serial);

    char data[50];
    size_t len = serializeJson(doc, data);
    DEBUG_PRINTLN(F("sending JSON string: "));
        for (int i = 0; i < len;  i++){
        DEBUG_PRINT(data[i]);
        }
        DEBUG_PRINTLN(F(" "));
    ws.textAll(data, len); 

}

void notifyClientsSingleObject(String object, boolean value) {
    StaticJsonDocument<50> doc;
    String key = object;
    boolean Value = value;
    doc[key] = Value; 

    char data[50];
    size_t len = serializeJson(doc, data);
    DEBUG_PRINTLN(F("sending JSON boolean: "));
        for (int i = 0; i < len;  i++){
        DEBUG_PRINT(data[i]);
        }
        DEBUG_PRINTLN(F(" "));
    ws.textAll(data, len); 
}

void notifyClientsSingleObjectByte(String object, byte value) {
    StaticJsonDocument<50> doc;
    String key = object;
    byte Value = value;
    doc[key] = Value; 

    char data[50];
    size_t len = serializeJson(doc, data);
        DEBUG_PRINTLN(F("sending JSON byte: "));
        for (int i = 0; i < len;  i++){
        DEBUG_PRINT(data[i]);
        }
        DEBUG_PRINTLN(F(" "));
    ws.textAll(data, len); 
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        DEBUG_PRINTLN(F("received JSON message: "));
        for (int i = 0; i < len; i++){
        //strval[i]=(char)data[i];    
        DEBUG_PRINT((char)data[i]);
        }
        DEBUG_PRINTLN(" ");

        //const uint8_t size = JSON_OBJECT_SIZE(1);
        StaticJsonDocument<200> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            DEBUG_PRINT(F("deserializeJson() failed with code "));
            DEBUG_PRINTLN(err.c_str());
            return;
        }

        if (json.containsKey("requestInfo")){sendProgramInfo();}
        else if (json.containsKey("hours")){hourStart = json["hours"]; minutesStart = json["minutes"]; syncTimeRTC(); } //
        else if (json.containsKey("saveInEEPROM")){saveInEEPROM = json["saveInEEPROM"];}   
        else if (json.containsKey("ssid")){String wifiID = json["ssid"]; EEPROMposition = offsetof(storeEEPROM, systemParam.SSID[0]); writeStringToEEPROM(EEPROMposition, wifiID); notifyClientsSingleString("wifiID", wifiID);}
        else if (json.containsKey("pass")){String wifiPASS = json["pass"]; EEPROMposition = offsetof(storeEEPROM, systemParam.PASS[0]); writeStringToEEPROM(EEPROMposition, wifiPASS); notifyClientsSingleString("wifiPASS", wifiPASS);}
        
        // THESE SYSTEM VALUES ARE ALWAYS SAVED TO EEPROM
        else if (json.containsKey("probeTypeT")){systemParam.probeTypeT = json["probeTypeT"]; EEPROM.put(offsetof(storeEEPROM, systemParam.probeTypeT), systemParam.probeTypeT);  EEPROM.commit(); }
        else if (json.containsKey("probeCountT")){systemParam.probeCountT = json["probeCountT"]; EEPROM.put(offsetof(storeEEPROM, systemParam.probeCountT), systemParam.probeCountT);  EEPROM.commit();}
        else if (json.containsKey("probeTypeH")){systemParam.probeTypeH = json["probeTypeH"]; EEPROM.put(offsetof(storeEEPROM, systemParam.probeTypeH), systemParam.probeTypeH);  EEPROM.commit();}
        else if (json.containsKey("probeCountH")){systemParam.probeCountH = json["probeCountH"]; EEPROM.put(offsetof(storeEEPROM, systemParam.probeCountH), systemParam.probeCountH);  EEPROM.commit();}
        else if (json.containsKey("relay1Connected")){relay1.connected = json["relay1Connected"]; EEPROM.put(offsetof(storeEEPROM, relay1.connected), relay1.connected);  EEPROM.commit();}
        else if (json.containsKey("relay2Connected")){relay2.connected = json["relay2Connected"]; EEPROM.put(offsetof(storeEEPROM, relay2.connected), relay2.connected);  EEPROM.commit();}
        else if (json.containsKey("relay3Connected")){relay3.connected = json["relay3Connected"]; EEPROM.put(offsetof(storeEEPROM, relay3.connected), relay3.connected);  EEPROM.commit();}
        else if (json.containsKey("relay4Connected")){relay4.connected = json["relay4Connected"]; EEPROM.put(offsetof(storeEEPROM, relay4.connected), relay4.connected);  EEPROM.commit();}
        else if (json.containsKey("relay5Connected")){relay5.connected = json["relay5Connected"]; EEPROM.put(offsetof(storeEEPROM, relay5.connected), relay5.connected);  EEPROM.commit();}
        else if (json.containsKey("relay6Connected")){relay6.connected = json["relay6Connected"]; EEPROM.put(offsetof(storeEEPROM, relay6.connected), relay6.connected);  EEPROM.commit();}
        
        else if (json.containsKey("fan1Connected")){fan1.connected = json["fan1Connected"]; EEPROM.put(offsetof(storeEEPROM, fan1.connected), fan1.connected);  EEPROM.commit();}
        else if (json.containsKey("fan2Connected")){fan2.connected = json["fan2Connected"]; EEPROM.put(offsetof(storeEEPROM, fan2.connected), fan2.connected);  EEPROM.commit();}
        else if (json.containsKey("graphUpdate")){systemParam.graphUpdate = json["graphUpdate"]; systemParam.graphUpdate = systemParam.graphUpdate*1000; EEPROM.put(offsetof(storeEEPROM, systemParam.graphUpdate), (systemParam.graphUpdate));  EEPROM.commit();}
        else if (json.containsKey("tempUpdate")){systemParam.tempUpdate = json["tempUpdate"]; systemParam.tempUpdate = systemParam.tempUpdate*1000; EEPROM.put(offsetof(storeEEPROM, systemParam.tempUpdate), (systemParam.tempUpdate));  EEPROM.commit();}
       
        else if (json.containsKey("measurements")){systemParam.measurements = json["measurements"];  EEPROM.put(offsetof(storeEEPROM, systemParam.measurements), (systemParam.measurements));  EEPROM.commit();}
        else if (json.containsKey("externalRTC")){systemParam.externalRTC = json["externalRTC"];  EEPROM.put(offsetof(storeEEPROM, systemParam.externalRTC), (systemParam.externalRTC));  EEPROM.commit(); ESP.restart();} // 
        else if (json.containsKey("resetRTC")){systemParam.resetRTC = json["resetRTC"]; syncTimeRTC();}//  EEPROM.put(offsetof(storeEEPROM, systemParam.resetRTC), (systemParam.resetRTC));  EEPROM.commit();}
        //else if (json.containsKey("PIDcontrol")){PIDcontrol = json["PIDcontrol"]; EEPROM.put(offsetof(storeInEEPROM, PIDcontrol), PIDcontrol);  EEPROM.commit();}

        else if (json.containsKey("funcRelay1")){relay1.function = json["funcRelay1"]; EEPROM.put(offsetof(storeEEPROM, relay1.function), relay1.function);  EEPROM.commit();}
        else if (json.containsKey("funcRelay2")){relay2.function = json["funcRelay2"]; EEPROM.put(offsetof(storeEEPROM, relay2.function), relay2.function);  EEPROM.commit();}
        else if (json.containsKey("funcRelay3")){relay3.function = json["funcRelay3"]; EEPROM.put(offsetof(storeEEPROM, relay3.function), relay3.function);  EEPROM.commit();}
        else if (json.containsKey("funcRelay4")){relay4.function = json["funcRelay4"]; EEPROM.put(offsetof(storeEEPROM, relay4.function), relay4.function);  EEPROM.commit();}
        else if (json.containsKey("funcRelay5")){relay5.function = json["funcRelay5"]; EEPROM.put(offsetof(storeEEPROM, relay5.function), relay5.function);  EEPROM.commit();}
        else if (json.containsKey("funcRelay6")){relay6.function = json["funcRelay6"]; EEPROM.put(offsetof(storeEEPROM, relay6.function), relay6.function);  EEPROM.commit();}
        
        else if (json.containsKey("lights1ON")){String StoreLights1ON = json["lights1ON"]; String StoreLights1OFF = json["lights1OFF"]; EEPROMposition = offsetof(storeEEPROM, lights1.lightsOn[0]); writeStringToEEPROM(EEPROMposition, StoreLights1ON); EEPROMposition = offsetof(storeEEPROM, lights1.lightsOff[0]); writeStringToEEPROM(EEPROMposition, StoreLights1OFF); reInitializeTimeStrings();}
        else if (json.containsKey("lights2ON")){String StoreLights2ON = json["lights2ON"]; String StoreLights2OFF = json["lights2OFF"]; EEPROMposition = offsetof(storeEEPROM, lights2.lightsOn[0]); writeStringToEEPROM(EEPROMposition, StoreLights2ON); EEPROMposition = offsetof(storeEEPROM, lights2.lightsOff[0]); writeStringToEEPROM(EEPROMposition, StoreLights2OFF); reInitializeTimeStrings();}
        else if (json.containsKey("lights3ON")){String StoreLights3ON = json["lights3ON"]; String StoreLights3OFF = json["lights3OFF"]; EEPROMposition = offsetof(storeEEPROM, lights3.lightsOn[0]); writeStringToEEPROM(EEPROMposition, StoreLights3ON); EEPROMposition = offsetof(storeEEPROM, lights3.lightsOff[0]); writeStringToEEPROM(EEPROMposition, StoreLights3OFF); reInitializeTimeStrings();}
        // THESE VALUES ARE ONLY PERMANENTLT SWITCHED WHEN EEPROM BUTTON IS CHECKED
        else if (json.containsKey("calValue1")){cal.pt100[0] = json["calValue1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.pt100[0]), cal.pt100[0]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue2")){cal.pt100[1] = json["calValue2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.pt100[1]), cal.pt100[1]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue3")){cal.pt100[2] = json["calValue3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.pt100[2]), cal.pt100[2]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue4")){cal.pt100[3] = json["calValue4"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.pt100[3]), cal.pt100[3]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue5")){cal.dhtT[0] = json["calValue5"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.dhtT[0]), cal.dhtT[0]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue6")){cal.dhtT[1] = json["calValue6"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.dhtT[1]), cal.dhtT[1]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue7")){cal.dhtH[0] = json["calValue7"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.dhtH[0]), cal.dhtH[0]);  EEPROM.commit(); };}
        else if (json.containsKey("calValue8")){cal.dhtH[1] = json["calValue8"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, cal.dhtH[1]), cal.dhtH[1]);  EEPROM.commit(); };}

        else if (json.containsKey("manualRelay1")){relay1.manual = json["manualRelay1"];}
        else if (json.containsKey("manualRelay2")){relay2.manual = json["manualRelay2"];}
        else if (json.containsKey("manualRelay3")){relay3.manual = json["manualRelay3"];}
        else if (json.containsKey("manualRelay4")){relay4.manual = json["manualRelay4"];}
        else if (json.containsKey("manualRelay5")){relay5.manual = json["manualRelay5"];}
        else if (json.containsKey("manualRelay6")){relay6.manual = json["manualRelay6"];}

        else if (json.containsKey("lights1")){climate1.lights = json["lights1"];} //lightState1 = !lights1;
        else if (json.containsKey("heater1")){climate1.heater = json["heater1"]; }// heaterState1 = !heater1;//  Serial.print("heaterState is now "); Serial.print(heaterState1); Serial.print(" And heater is: "); Serial.println(heater1);
        else if (json.containsKey("fan1")){climate1.fan = json["fan1"]; fan1.fanState = !climate1.fan;}
        else if (json.containsKey("humidifier1")){climate1.humidifier = json["humidifier1"]; } // humidifierState1 = !humidifier1;

        else if (json.containsKey("lights2")){climate2.lights = json["lights2"]; } // lightState2 = !lights2;}
        else if (json.containsKey("heater2")){climate2.heater = json["heater2"]; }// heaterState2 = !heater2;// Serial.print("heaterState is now "); Serial.print(heaterState1); Serial.print(" And heater is: "); Serial.println(heater1);
        else if (json.containsKey("fan2")){climate2.fan = json["fan2"]; fan2.fanState = !climate2.fan;}
        else if (json.containsKey("humidifier2")){climate2.humidifier = json["humidifier2"]; } // humidifierState2 = !humidifier2;

        else if (json.containsKey("lights3")){climate3.lights = json["lights3"]; } // lightState2 = !lights2;}
        else if (json.containsKey("heater3")){climate3.heater = json["heater3"]; }// heaterState2 = !heater2;// Serial.print("heaterState is now "); Serial.print(heaterState1); Serial.print(" And heater is: "); Serial.println(heater1);
        //else if (json.containsKey("humidifier3")){humidifier3 = json["humidifier3"]; } // humidifierState2 = !humidifier2;

        else if (json.containsKey("manualMosfet1")){fan1.manual = json["manualMosfet1"]; if (!fan1.manual){fan1.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan1.manualSpeed));}; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan1.manual), fan1.manual);  EEPROM.commit(); };}
        else if (json.containsKey("manualFanspeed1")){fan1.manualSpeed = json["manualFanspeed1"]; if (fan1.manual){ if (fan1.manualSpeed > 0){climate1.fan = true;} else {climate1.fan = false;} fan1.fanState = !climate1.fan;}; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan1.manualSpeed), fan1.manualSpeed);  EEPROM.commit(); };}
        else if (json.containsKey("manualMosfet2")){fan2.manual = json["manualMosfet2"]; if (!fan2.manual){fan2.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan2.manualSpeed));}; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan2.manual), fan2.manual);  EEPROM.commit(); };}
        else if (json.containsKey("manualFanspeed2")){fan2.manualSpeed = json["manualFanspeed2"]; if (fan2.manual){if (fan2.manualSpeed > 0){climate2.fan = true;} else {climate2.fan = false;} fan2.fanState = !climate2.fan;}; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan2.manualSpeed), fan2.manualSpeed);  EEPROM.commit(); };}
        else if (json.containsKey("fan1daySwitch")){fan1.daySwitch = json["fan1daySwitch"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan1.daySwitch), fan1.daySwitch);  EEPROM.commit(); };}
        else if (json.containsKey("fan2daySwitch")){fan2.daySwitch = json["fan2daySwitch"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan2.daySwitch), fan2.daySwitch);  EEPROM.commit(); };}

        else if (json.containsKey("OUTPUT_MIN1")){fan1.output_min = json["OUTPUT_MIN1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan1.output_min), fan1.output_min);  EEPROM.commit(); };}
        else if (json.containsKey("OUTPUT_MAX1")){fan1.output_max = json["OUTPUT_MAX1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan1.output_max), fan1.output_max);  EEPROM.commit(); };}
        else if (json.containsKey("OUTPUT_MIN2")){fan2.output_min = json["OUTPUT_MIN2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan2.output_min), fan2.output_min);  EEPROM.commit(); };}
        else if (json.containsKey("OUTPUT_MAX2")){fan2.output_max = json["OUTPUT_MAX2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, fan2.output_max), fan2.output_max);  EEPROM.commit(); };}

        // else if (json.containsKey("KP")){KP = json["KP"]; if (saveInEEPROM){EEPROM.put(offsetof(storeInEEPROM, KP), KP);  EEPROM.commit(); };}
        // else if (json.containsKey("KI")){KI = json["KI"]; if (saveInEEPROM){EEPROM.put(offsetof(storeInEEPROM, KI), KI);  EEPROM.commit(); };}
        // else if (json.containsKey("KD")){KD = json["KD"]; if (saveInEEPROM){EEPROM.put(offsetof(storeInEEPROM, KD), KD);  EEPROM.commit(); };}

        else if (json.containsKey("humidMin1")){humidParam1.humidmin = json["humidMin1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, humidParam1.humidmin), humidParam1.humidmin);  EEPROM.commit(); };}
        else if (json.containsKey("humidMax1")){humidParam1.humidmax = json["humidMax1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, humidParam1.humidmax), humidParam1.humidmax);  EEPROM.commit(); };}
        else if (json.containsKey("humidMin2")){humidParam2.humidmin = json["humidMin2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, humidParam2.humidmin), humidParam2.humidmin);  EEPROM.commit(); };}
        else if (json.containsKey("humidMax2")){humidParam2.humidmax = json["humidMax2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, humidParam2.humidmax), humidParam2.humidmax);  EEPROM.commit(); };}
        //else if (json.containsKey("humidMin3")){humidMin3 = json["humidMin3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeInEEPROM, humidMin3), humidMin3);  EEPROM.commit(); };}
        //else if (json.containsKey("humidMax3")){humidMax3 = json["humidMax3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeInEEPROM, humidMax3), humidMax3);  EEPROM.commit(); };}        

        else if (json.containsKey("daySoilTemp1")){tempParam1.daySoilTemp = json["daySoilTemp1"]; tempParam1.targetSoilTemp = tempParam1.daySoilTemp; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam1.daySoilTemp), tempParam1.daySoilTemp);  EEPROM.commit(); };}
        else if (json.containsKey("nightSoilTemp1")){tempParam1.nightSoilTemp = json["nightSoilTemp1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam1.nightSoilTemp), tempParam1.nightSoilTemp);  EEPROM.commit(); };}
        else if (json.containsKey("tempRange1")){tempParam1.tempRange = json["tempRange1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam1.tempRange), tempParam1.tempRange);  EEPROM.commit(); };}
        else if (json.containsKey("alarmRange1")){tempParam1.alarmRange = json["alarmRange1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam1.alarmRange), tempParam1.alarmRange);  EEPROM.commit(); };}
        else if (json.containsKey("soilAlarm1")){tempParam1.soilAlarm = json["soilAlarm1"];}
        else if (json.containsKey("targetAirTemp1")){tempParam1.targetAirTemp = json["targetAirTemp1"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam1.targetAirTemp), tempParam1.targetAirTemp);  EEPROM.commit(); };}
        else if (json.containsKey("airAlarm1")){tempParam1.airAlarm = json["airAlarm1"];}

        else if (json.containsKey("daySoilTemp2")){tempParam2.daySoilTemp = json["daySoilTemp2"]; tempParam2.targetSoilTemp = tempParam2.daySoilTemp; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam2.daySoilTemp), tempParam2.daySoilTemp);  EEPROM.commit(); };}
        else if (json.containsKey("nightSoilTemp2")){tempParam2.nightSoilTemp = json["nightSoilTemp2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam2.nightSoilTemp), tempParam2.nightSoilTemp);  EEPROM.commit(); };}        
        else if (json.containsKey("tempRange2")){tempParam2.tempRange = json["tempRange2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam2.tempRange), tempParam2.tempRange);  EEPROM.commit(); };}
        else if (json.containsKey("alarmRange2")){tempParam2.alarmRange = json["alarmRange2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam2.alarmRange), tempParam2.alarmRange);  EEPROM.commit(); };}
        else if (json.containsKey("soilAlarm2")){tempParam2.soilAlarm = json["soilAlarm2"];}
        else if (json.containsKey("targetAirTemp2")){tempParam2.targetAirTemp = json["targetAirTemp2"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam2.targetAirTemp), tempParam2.targetAirTemp);  EEPROM.commit(); };}
        else if (json.containsKey("airAlarm2")){tempParam2.airAlarm = json["airAlarm2"];}

        else if (json.containsKey("daySoilTemp3")){tempParam3.daySoilTemp = json["daySoilTemp3"]; tempParam3.targetSoilTemp = tempParam3.daySoilTemp; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam3.daySoilTemp), tempParam3.daySoilTemp);  EEPROM.commit(); };}
        else if (json.containsKey("nightSoilTemp3")){tempParam3.nightSoilTemp = json["nightSoilTemp3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam3.nightSoilTemp), tempParam3.nightSoilTemp);  EEPROM.commit(); };}        
        else if (json.containsKey("tempRange3")){tempParam3.tempRange = json["tempRange3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam3.tempRange), tempParam3.tempRange);  EEPROM.commit(); };}
        else if (json.containsKey("alarmRange3")){tempParam3.alarmRange = json["alarmRange3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam3.alarmRange), tempParam3.alarmRange);  EEPROM.commit(); };}
        else if (json.containsKey("soilAlarm3")){tempParam3.soilAlarm = json["soilAlarm3"];}

        else if (json.containsKey("daySoilTemp4")){tempParam4.daySoilTemp = json["daySoilTemp4"]; tempParam4.targetSoilTemp = tempParam4.daySoilTemp; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam4.daySoilTemp), tempParam4.daySoilTemp);  EEPROM.commit(); };}
        else if (json.containsKey("nightSoilTemp4")){tempParam4.nightSoilTemp = json["nightSoilTemp4"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam4.nightSoilTemp), tempParam4.nightSoilTemp);  EEPROM.commit(); };}        
        else if (json.containsKey("tempRange4")){tempParam4.tempRange = json["tempRange4"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam4.tempRange), tempParam4.tempRange);  EEPROM.commit(); };}
        else if (json.containsKey("alarmRange4")){tempParam4.alarmRange = json["alarmRange4"]; if (saveInEEPROM){EEPROM.put(offsetof(storeEEPROM, tempParam4.alarmRange), tempParam4.alarmRange);  EEPROM.commit(); };}
        else if (json.containsKey("soilAlarm4")){tempParam4.soilAlarm = json["soilAlarm4"];}        
        //else if (json.containsKey("targetAirTemp3")){targetAirTemp3 = json["targetAirTemp3"]; if (saveInEEPROM){EEPROM.put(offsetof(storeInEEPROM, targetAirTemp3), targetAirTemp3);  EEPROM.commit(); };}
        //else if (json.containsKey("airAlarm3")){airAlarm3 = json["airAlarm3"];}        
        else if (json.containsKey("Reboot")){ESP.restart();}
        notifyClientsSingleObject("recMsg", true);
    }
}  

void sendProgramInfo(){
//StaticJsonDocument<2000> doc; // 2500
DynamicJsonDocument doc(2000);
doc["probeTypeT"] = systemParam.probeTypeT;
doc["probeCountT"]   = systemParam.probeCountT;
doc["probeTypeH"]   = systemParam.probeTypeH;
doc["probeCountH"]   = systemParam.probeCountH;
doc["graphUpdate"]   = systemParam.graphUpdate/1000;
doc["tempUpdate"]   = systemParam.tempUpdate/1000;
doc["measurements"] = systemParam.measurements;
doc["externalRTC"] = systemParam.externalRTC;
doc["resetRTC"] = systemParam.resetRTC;

// doc["PIDcontrol"]   = PIDcontrol;

doc["calV1"]   = String(cal.pt100[0],2);
doc["calV2"]   = String(cal.pt100[1],2);
doc["calV3"]   = String(cal.pt100[2],2);
doc["calV4"]   = String(cal.pt100[3],2);
doc["calV5"]   = String(cal.dhtT[0],2);
doc["calV6"]   = String(cal.dhtT[1],2);
doc["calV7"]   = String(cal.dhtH[0],2);
doc["calV8"]   = String(cal.dhtH[1],2);

doc["relay1c"]   = relay1.connected;
doc["relay2c"]   = relay2.connected;
doc["relay3c"]   = relay3.connected;
doc["relay4c"]   = relay4.connected;
doc["relay5c"]   = relay5.connected;
doc["relay6c"]   = relay6.connected;

doc["relay1m"] = relay1.manual;
doc["relay2m"] = relay2.manual;
doc["relay3m"] = relay3.manual;
doc["relay4m"] = relay4.manual;
doc["relay5m"] = relay5.manual;
doc["relay6m"] = relay6.manual;

doc["relay1f"] = relay1.function;
doc["relay2f"] = relay2.function;
doc["relay3f"] = relay3.function;
doc["relay4f"] = relay4.function;
doc["relay5f"] = relay5.function;
doc["relay6f"] = relay6.function;

doc["OUTPUT_MIN1"]   = fan1.output_min;
doc["OUTPUT_MAX1"]   = fan1.output_max;
doc["OUTPUT_MIN2"]   = fan2.output_min;
doc["OUTPUT_MAX2"]   = fan2.output_max;
// doc["KP"]   = String(KP,2); // may require more decimals
// doc["KI"]   = String(KI,2); // may require more decimals
// doc["KD"]   = String(KD,2); // may require more decimals

doc["humidMin1"] = humidParam1.humidmin;
doc["humidMax1"] = humidParam1.humidmax;
doc["humidMin2"] = humidParam2.humidmin;
doc["humidMax2"] = humidParam2.humidmax;
// doc["humidMin3"] = humidMin3;
// doc["humidMax3"] = humidMax3;
doc["daySoilTemp1"] = String(tempParam1.daySoilTemp,2);
doc["nightSoilTemp1"] = String(tempParam1.nightSoilTemp,2);
doc["tempRange1"] = String(tempParam1.tempRange,2);
doc["alarmRange1"] = String(tempParam1.alarmRange,2);
doc["soilAlarm1"] = tempParam1.soilAlarm;
doc["targetAirTemp1"] = String(tempParam1.targetAirTemp,2);
doc["airAlarm1"] = tempParam1.airAlarm;

doc["daySoilTemp2"] = String(tempParam2.daySoilTemp,2);
doc["nightSoilTemp2"] = String(tempParam2.nightSoilTemp,2);
doc["tempRange2"] = String(tempParam2.tempRange,2);
doc["alarmRange2"] = String(tempParam2.alarmRange,2);
doc["soilAlarm2"] = tempParam2.soilAlarm;
doc["targetAirTemp2"] = String(tempParam2.targetAirTemp,2);
doc["airAlarm2"] = tempParam2.airAlarm;

doc["daySoilTemp3"] = String(tempParam3.daySoilTemp,2);
doc["nightSoilTemp3"] = String(tempParam3.nightSoilTemp,2);
doc["tempRange3"] = String(tempParam3.tempRange,2);
doc["alarmRange3"] = String(tempParam3.alarmRange,2);
doc["soilAlarm3"] = tempParam3.soilAlarm;

doc["daySoilTemp4"] = String(tempParam4.daySoilTemp,2);
doc["nightSoilTemp4"] = String(tempParam4.nightSoilTemp,2);
doc["tempRange4"] = String(tempParam4.tempRange,2);
doc["alarmRange4"] = String(tempParam4.alarmRange,2);
doc["soilAlarm4"] = tempParam4.soilAlarm;
//doc["targetAirTemp3"] = String(targetAirTemp3,2);
//doc["airAlarm3"] = airAlarm3;
doc["fan1c"] = fan1.connected;
doc["fan2c"] = fan2.connected;

doc["fan1m"] = fan1.manual;
doc["manualFanspeed1"] = fan1.manualSpeed;
doc["fan2m"] = fan2.manual;
doc["manualFanspeed2"] = fan2.manualSpeed;
doc["fan1daySwitch"] = fan1.daySwitch;
doc["fan2daySwitch"] = fan2.daySwitch;
doc["fanspeed1"] = fanspeed1;
doc["fanspeed2"] = fanspeed2;

doc["lights1ON"] = schedule1.lightsON;
doc["lights1OFF"] = schedule1.lightsOFF;
doc["lights2ON"] = schedule2.lightsON;
doc["lights2OFF"] = schedule2.lightsOFF;
doc["lights3ON"] = schedule3.lightsON;
doc["lights3OFF"] = schedule3.lightsOFF;

doc["lights1"] = climate1.lights;
doc["heater1"] = climate1.heater;
doc["fan1"] = climate1.fan;
doc["humidifier1"] = climate1.humidifier;

doc["lights2"] = climate2.lights;
doc["heater2"] = climate2.heater;
doc["fan2"] = climate2.fan;
doc["humidifier2"] = climate2.humidifier;

doc["lights3"] = climate3.lights;
doc["heater3"] = climate3.heater;

doc["heater4"] = climate4.heater;
//doc["fan3"] = fan3;
//doc["humidifier3"] = humidifier3;
doc["saveInEEPROM"] = saveInEEPROM;

char data[2000]; // 2500
size_t len = serializeJson(doc, data);

    DEBUG_PRINT(F("sending JSON message: ")); DEBUG_PRINTLN(len);
    for (int i = 0; i < len;  i++){
    DEBUG_PRINT(data[i]);
    }
    DEBUG_PRINTLN(" ");

ws.textAll(data, len);
return;
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite){
  if (strToWrite == NULL){
      //Serial.println("this string is empty");
  }
  else{  
    byte length = strToWrite.length();
    //Serial.print("len "); Serial.println(len);
    //Serial.print("addr offset ");Serial.println(addrOffset);
    //EEPROM.write(addrOffset, len);
    for (int i = 0; i < length; i++){
        //EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
        EEPROM.write(addrOffset + i, strToWrite[i]);
        DEBUG_PRINT(strToWrite[i]);
    }
    DEBUG_PRINTLN(" ");
    EEPROM.write((length+addrOffset),'\0');
    //EEPROM.write((length+addrOffset),NULL);
    EEPROM.commit();
  }
return;
} 


// String readStringFromEEPROM(int addrOffset)
// {
//   int newStrLen = EEPROM.read(addrOffset);
//   char data[newStrLen + 1];
//   for (int i = 0; i < newStrLen; i++)
//   {
//     data[i] = EEPROM.read(addrOffset + 1 + i);
//   }
//   data[newStrLen] = '\0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
//   return String(data);
// }

void sendTempToClient (){
    StaticJsonDocument<100> doc;
    doc["temp1"] = String(valid.pt100[0],2);
    doc["temp2"] = String(valid.pt100[1],2);
    doc["temp3"] = String(valid.pt100[2],2);
    doc["temp4"] = String(valid.pt100[3],2);

    char data[100];
    size_t len = serializeJson(doc, data);

    DEBUG_PRINT(F("length temp string: ")); DEBUG_PRINTLN(len);
    for (int i = 0; i < len;  i++){
    //DEBUG_PRINT(data[i]);
    }
    //DEBUG_PRINTLN(" ");

    ws.textAll(data, len);
    return;
}

void sendHumidityToClient(){
    StaticJsonDocument<200> doc;
    doc["temp6"] =  String(valid.dhtT[0],2);
    doc["temp7"] =  String(valid.dhtT[1],2);
    //doc["temperature8"] =  String(dhtT[2],2);
    doc["humid1"] =  String(valid.dhtH[0],2);
    doc["humid2"] =  String(valid.dhtH[1],2);
    //doc["humid3"] =  String(humidity[2],2);

    char data[200];
    size_t len = serializeJson(doc, data);

    DEBUG_PRINT(F("length humidity string: ")); DEBUG_PRINTLN(len);
    for (int i = 0; i < len;  i++){
    //DEBUG_PRINT(data[i]);
    }
    //DEBUG_PRINTLN(F(" "));

    ws.textAll(data, len);
    return;
}

void updateGraph (){
    if (systemParam.probeCountT > 2){
        notifyClientsSingleObject("updateGraph", true);
    }
    else {
        notifyClientsSingleObject("updateGraph", false);
    }                
}
