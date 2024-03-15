void executeTask(byte function, boolean manualRelay, const int relayPin){
  switch (function){
    case 0: noFunction(manualRelay, relayPin); break; // do nothing - no function attached
    case 1: heater1Control(manualRelay, relayPin); break;
    case 2: heater2Control(manualRelay, relayPin); break;
    case 3: heater3Control(manualRelay, relayPin); break;
    case 4: heater4Control(manualRelay, relayPin); break;
    case 5: light1Control(manualRelay, relayPin); break;
    case 6: light2Control(manualRelay, relayPin); break;
    case 7: light3Control(manualRelay, relayPin); break;
    case 8: humidity1Control(manualRelay, relayPin); break;
    case 9: humidity2Control(manualRelay, relayPin); break;
    case 10: DHTsensors(manualRelay, relayPin); break;
    // case 10: humidity3Control(manualRelay, relayPin); break;
    default:    break;
    return;
}
}

void noFunction (boolean manualRelay, const int relayPin){
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
      if (manualRelay != relayReg){ // checks if manualRelay is not equal to relay output. If it's not: if manualRelay is true turn On Relay, if false, turn Off relay.
        digitalWrite(relayPin, !manualRelay);
      }
}

void light1Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (currentMinutes > schedule1.minutesLightsOn && currentMinutes < schedule1.minutesLightsOff){
      climate1.lights = true;
      tempParam1.targetSoilTemp = tempParam1.daySoilTemp;
      // if (!fan1.manual){
      //   if (fan1.daySwitch){
      //     fan1.manualSpeed = fan1.output_min;
      //   }
      //   else {
      //     //fan1.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan1.manualSpeed));
      //     fan1.manualSpeed = storedFanSpeed1;
      //   }
      // }
    }
    else if (currentMinutes < schedule1.minutesLightsOn || currentMinutes > schedule1.minutesLightsOn){
      climate1.lights = false;
      tempParam1.targetSoilTemp = tempParam1.nightSoilTemp;
      // if (!fan1.manual){
      // //fan1.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan1.manualSpeed));
      // fan1.manualSpeed = storedFanSpeed1;
      // }
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
      if (climate1.lights != relayReg){
        digitalWrite(relayPin, !climate1.lights);
        //Serial.println("send lights message to client");
        notifyClientsSingleObject("lights1", climate1.lights);
      }
  return;
}

void light2Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (currentMinutes > schedule2.minutesLightsOn && currentMinutes < schedule2.minutesLightsOff){
      climate2.lights = true;
      tempParam2.targetSoilTemp = tempParam2.daySoilTemp;
      // if (!fan2.manual){
      //   if (fan2.daySwitch){
      //     fan2.manualSpeed = fan2.output_min;
      //   }
      //   else {
      //     //fan2.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan2.manualSpeed));
      //     fan2.manualSpeed = storedFanSpeed2;
      //   }
      // }
    }
    else if (currentMinutes < schedule2.minutesLightsOn || currentMinutes > schedule2.minutesLightsOn){
      climate2.lights = false;
      tempParam2.targetSoilTemp = tempParam2.nightSoilTemp;
      // if (!fan2.manual){
      // //fan2.manualSpeed = EEPROM.read(offsetof(storeEEPROM, fan2.manualSpeed));
      // fan2.manualSpeed = storedFanSpeed2;
      // }
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
      if (climate2.lights != relayReg){
        digitalWrite(relayPin, !climate2.lights);
        //Serial.println("send lights message to client");
        notifyClientsSingleObject("lights2", climate2.lights);
      }
  return;
}

  void light3Control(boolean manualRelay, const int relayPin){
 if (!manualRelay){
    if (currentMinutes > schedule3.minutesLightsOn && currentMinutes < schedule3.minutesLightsOff){
      climate3.lights = true;
      tempParam3.targetSoilTemp = tempParam3.daySoilTemp;
      tempParam4.targetSoilTemp = tempParam4.daySoilTemp;
      //Serial.println("LIGHTS2_ON");
    }
    else if (currentMinutes < schedule3.minutesLightsOn || currentMinutes > schedule3.minutesLightsOn){
      climate3.lights = false;
      tempParam3.targetSoilTemp = tempParam3.nightSoilTemp;
      tempParam4.targetSoilTemp = tempParam4.nightSoilTemp;
      //Serial.println("LIGHTS2_OFF");
    }
  } 
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));

  if (climate3.lights != relayReg){
    //lightState2 = lights2;
    digitalWrite(relayPin, !climate3.lights);
    //Serial.println("send lights message to client");
    notifyClientsSingleObject("lights3", climate3.lights);
  }
  return;
  }

  void heater1Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (valid.pt100[0] < tempParam1.targetSoilTemp - tempParam1.tempRange){
      climate1.heater = true;
    }
    else if (valid.pt100[0] > tempParam1.targetSoilTemp + tempParam1.tempRange){
      climate1.heater = false;
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
  if (climate1.heater != relayReg){
    //heaterState1 = heater1;
    digitalWrite(relayPin, !climate1.heater);
    notifyClientsSingleObject("heater1", climate1.heater);
  }
  return;
}


void heater2Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (valid.pt100[1] < tempParam2.targetSoilTemp - tempParam2.tempRange){
      climate2.heater = true;
    }
    else if (valid.pt100[1] > tempParam2.targetSoilTemp + tempParam2.tempRange){
      climate2.heater = false;
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
  if (climate2.heater != relayReg){
    //heaterState2 = heater2;
    digitalWrite(relayPin, !climate2.heater);
    notifyClientsSingleObject("heater2", climate2.heater);
  }
  return;
}

void heater3Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (valid.pt100[2] < tempParam3.targetSoilTemp - tempParam3.tempRange){
      climate3.heater = true;
    }
    else if (valid.pt100[2] > tempParam3.targetSoilTemp + tempParam3.tempRange){
      climate3.heater = false;
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
  if (climate3.heater != relayReg){
    //heaterState2 = heater2;
    digitalWrite(relayPin, !climate3.heater);
    notifyClientsSingleObject("heater3", climate3.heater);
  }
  return;
}

void heater4Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (valid.pt100[3] < tempParam4.targetSoilTemp - tempParam4.tempRange){
      climate4.heater = true;
    }
    else if (valid.pt100[3] > tempParam4.targetSoilTemp + tempParam4.tempRange){
      climate4.heater = false;
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
  if (climate4.heater != relayReg){
    //heaterState2 = heater2;
    digitalWrite(relayPin, !climate4.heater);
    notifyClientsSingleObject("heater4", climate4.heater);
  }
  return;
}

void humidity1Control(boolean manualRelay, const int relayPin){
  if (!manualRelay){
    if (valid.dhtH[0] < humidParam1.humidmin){
      climate1.humidifier = true;
    }
    else if (valid.dhtH[0] > humidParam1.humidmax){
      climate1.humidifier = false;
    }
  }
  bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
    //if (humidifierState1 != humidifier1){
    if (climate1.humidifier != relayReg){
      //humidifierState1 = humidifier1;
      digitalWrite(relayPin, !climate1.humidifier);
      notifyClientsSingleObject("humidifier1", climate1.humidifier);
    }
    return;
}

void humidity2Control(boolean manualRelay, const int relayPin){
if (!manualRelay){
  if (valid.dhtH[1] < humidParam2.humidmin){
    climate2.humidifier = true;
  }
  else if (valid.dhtH[1] > humidParam1.humidmax){
    climate2.humidifier = false;
  }
}
bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
  // if (humidifierState2 != humidifier2){
  //   humidifierState2 = humidifier2;
  if (climate2.humidifier != relayReg){
    digitalWrite(relayPin, !climate2.humidifier);
    notifyClientsSingleObject("humidifier2", climate2.humidifier);
  }
  return;
}

void fan1Control(){ // CLIMATE 1 based on TargetAirTemp1 and value from DHT22 sensor.
if (fan1.manual){
  if (climate1.fan){
    fan1.speed = fan1.manualSpeed;
    }
  else {fan1.speed = 0;
  }
  if (msgFanState1){
    messageFanState1();
  }
}
else if (!fan1.manual){
  climate1.fan = true;
  // if (!PIDcontrol){
    if (climate1.humidifier){
      fan1.speed = fan1.output_min;
    }
    else {
      if (valid.dhtT[0] > tempParam1.targetAirTemp + tempParam1.alarmRange){
        fan1.speed = fan1.output_max;
      }
      else if (valid.dhtT[0] < tempParam1.targetAirTemp){ //  - tempRange1
        if ((climate1.lights) && (fan1.daySwitch)){
          fan1.speed = fan1.output_min; // OUTPUT_MIN1
        }
        else {
          fan1.speed = fan1.manualSpeed;
        }
      }
      // else if (dhtTemp[0] > targetAirTemp1 - tempRange1 && dhtTemp[0] < targetAirTemp1 + tempRange1){
      //   outputVal1 = manualFanspeed1;
      // }
      else if (valid.dhtT[0] > tempParam1.targetAirTemp && valid.dhtT[0] < tempParam1.targetAirTemp + tempParam1.alarmRange){
        fan1.speed = modifiedMap((valid.dhtT[0]-tempParam1.targetAirTemp), 0, tempParam1.alarmRange, fan1.output_min, fan1.output_max);
      }
      }
  //}
  // else {
  //     temperature=dhtTemp[0];
  //     myPID.run(); //call every loop, updates automatically at certain time interval
  // }
  if (fan1.speed == 0){
    climate1.fan = false;
  }
  else {
    climate1.fan = true;
  }
}

if (fan1.fanState != climate1.fan || msgFanState1 == true){
  fan1.fanState = climate1.fan;
  messageFanState1();
}
 if (fan1.speed != fan1.oldSpeed){
  //Serial.println(outputVal);
  fan1.oldSpeed = fan1.speed;   
  //Serial.println(outputVal1);
  ledcWrite(ledChannel1, fan1.speed);
  fanspeed1 = map(fan1.speed, 0, 255, 0, 100);
  //Serial.println(fanspeed1);
  notifyClientsSingleObjectByte("fanspeed1", fanspeed1);   
  }
}

void messageFanState1(){
//digitalWrite(ONBOARD_LED, fan1);
msgFanState1 = false;
notifyClientsSingleObject("fan1", climate1.fan);
return;
}

void fan2Control(){ // CLIMATE 1 based on TargetAirTemp1 and value from DHT22 sensor.
if (fan2.manual){
  if (climate2.fan){
    fan2.speed = fan2.manualSpeed;
    }
  else {
    fan2.speed = 0;
    }
  if (msgFanState2){
    messageFanState2();
  }
}
else if (!fan2.manual){
    climate2.fan = true;
    if (climate2.humidifier){
      fan2.speed = fan2.output_min;
    }
    else {
      if (valid.dhtT[1] > tempParam2.targetAirTemp + tempParam2.alarmRange){
        fan2.speed = fan2.output_max;
      }
      else if (valid.dhtT[1] < tempParam2.targetAirTemp){ //  - tempRange2
        if ((climate2.lights) && (fan2.daySwitch)){
          fan2.speed = fan2.output_min; // OUTPUT_MIN1
        }
        else {
          fan2.speed = fan2.manualSpeed;
        }
      }
      // else if (dhtTemp[1] > targetAirTemp2 - tempRange2 && dhtTemp[1] < targetAirTemp2 + tempRange2){
      //   outputVal2 = manualFanspeed2;
      // }
      else if (valid.dhtT[1] > tempParam2.targetAirTemp && valid.dhtT[1] < tempParam2.targetAirTemp + tempParam2.alarmRange){
        fan2.speed = modifiedMap((valid.dhtT[1]-tempParam2.targetAirTemp), 0, tempParam2.alarmRange, fan2.output_min, fan2.output_max);
      }
    if (fan2.speed == 0){
      climate2.fan = false;
    }
    else{
      climate2.fan = true;
    }   
  }
}

if (fan2.fanState != climate2.fan || msgFanState2 == true){
fan2.fanState = climate2.fan;
messageFanState2();
}
 if (fan2.speed != fan2.oldSpeed){
  //Serial.println(outputVal);
  fan2.oldSpeed = fan2.speed;
  ledcWrite(ledChannel2, fan2.speed);
  fanspeed2 = map(fan2.speed, 0, 255, 0, 100);
  notifyClientsSingleObjectByte("fanspeed2", fanspeed2);   
  }
}

void messageFanState2(){
msgFanState2 = false;
notifyClientsSingleObject("fan2", climate2.fan);
return;
}

void DHTsensors(boolean manualRelay, const int relayPin){
bool relayReg = !(*portOutputRegister( digitalPinToPort(relayPin) ) & digitalPinToBitMask(relayPin));
if (relayReg == 0 && (sensorsOff)){
  digitalWrite(relayPin, false); // boolean resetHumidity = false;
  sensorsOff = false;
}

if (resetHumidity){
  digitalWrite(relayPin, true); // boolean resetHumidity is also true
    resetHumidity = false;
    humidityCounter = 0;
    sensorsOff = true;
    resetCount = resetCount + 1;
}
}