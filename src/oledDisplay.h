void displayOledScreen(float temp1, float temp2, float temp3, float temp4){
  oledPage = oledPage + 1;
  if (oledPage > oledMaxPage){
    oledPage = 1;
  }
  //displayTime = !displayTime;
  //if (!displayTime){
  if (oledPage == 1){
    display.clearDisplay();
    display.setTextSize(1);
    char buffer[40];
    if (systemParam.probeCountT >= 1){
    display.setCursor(0,8);
    sprintf(buffer, "T1: %2.1f   %2.1f", temp1, cal.pt100[0]);  
    display.print(buffer); 
    }
    if (systemParam.probeCountT >= 2){
    display.setCursor(0,16);
    sprintf(buffer, "T2: %2.1f   %2.1f", temp2, cal.pt100[1]);  
    display.print(buffer);
    }
    if (systemParam.probeCountT >= 3){
    display.setCursor(0,24);
    sprintf(buffer, "T3: %2.1f   %2.1f", temp3, cal.pt100[2]);  
    display.print(buffer);
    }
    if (systemParam.probeCountT >= 4){
    display.setCursor(0,32);
    sprintf(buffer, "T4: %2.1f   %2.1f", temp4, cal.pt100[3]);  
    display.print(buffer);
    }
    if (systemParam.probeCountH >= 1){
    display.setCursor(0,40);
    sprintf(buffer, "DHT1: %2.1f   %2.1f", valid.dhtT[0], valid.dhtH[0]);  
    display.print(buffer);
    }
    if (systemParam.probeCountH >= 1){ 
    display.setCursor(0,48);
    sprintf(buffer, "DHT2: %2.1f   %2.1f", valid.dhtT[1], valid.dhtH[1]);  
    display.print(buffer);
    }

    //   display.setCursor(0,10);
    // display.print("t1");
    // display.setCursor(0,46); // 54 is fit
    // display.print("soil");
    // display.setCursor(96,8); 
    // display.print(targetSoilTemp1); display.print((char)247); //display.print((char)247);
    // display.setCursor(96,16); 
    // display.print(fanspeed1); display.print("%");
    // uint8_t calposition = 54-(probeCountT*8);
    // display.setCursor(96, calposition);
    // display.print("cal:");
    // for (int sensor = 0; sensor < probeCountT; sensor++){
    //   uint8_t yposition = (calposition+8)+sensor*8;
    //   display.setCursor(96, yposition);
    //   display.print(calibrationValue[sensor], 1);
    // }

    display.setCursor(0,0);
    // if (alarmMessage == 0){ 
    if (wifiStationMode){
    //display.print("wifi "); display.println(WiFi.localIP());
    display.print("wifi "); display.print(WiFi.localIP());  
    }
    else {
    display.print("soft-AP "); display.println(WiFi.softAPIP());  
    } 
    // }
    // else if (alarmMessage > 0){
    //   switch(alarmMessage){
    //     case 1: {display.print("reached targetTemp");} break;
    //     case 2: {display.print("exceeded tempOffset");} break;
    //     case 3: {display.print("reached alarmTimer");} break;
    //   }
    // }

    // if (probeCountT > 2){
    //   display.setCursor(0,56);
    //   display.print("T3:"); display.print(temp3, 0);
    //     if (probeCountT > 3){
    //     display.print(" T4:"); display.print(temp4, 0);  
    //     }
    // }
    // display.setTextSize(3); 
    // display.setCursor(24,8);
    // display.print(temp1, 0); display.setTextSize(1); display.println((char)247);
    // display.setCursor(24,32); 
    // display.setTextSize(3);
    // display.print(temp2, 0); display.setTextSize(1); display.println((char)247);
    display.display();
  }
  //else if (displayTime){
  else if (oledPage == 2){  
    display.clearDisplay();
    display.setCursor(0,0);
    char buffer[40];
    sprintf(buffer, "time %02d:%02d   / %d", dateHour, dateMinute, resetCount);   
    display.print(buffer);
    display.setCursor(0,10);
    //char buffer2[40];
    sprintf(buffer, "light1 %u-%u: %u", schedule1.minutesLightsOn, schedule1.minutesLightsOff, currentMinutes); 
    display.print(buffer); 
    display.setCursor(0,18);
    //char buffer2[40];
    sprintf(buffer, "light2 %u-%u: %u", schedule2.minutesLightsOn, schedule2.minutesLightsOff, currentMinutes); 
    display.print(buffer);
    display.setCursor(0,28);
    // Reads pin settings from register
    bool relayReg1 = !(*portOutputRegister( digitalPinToPort(RELAYPIN1) ) & digitalPinToBitMask(RELAYPIN1));
    bool relayReg2 = !(*portOutputRegister( digitalPinToPort(RELAYPIN2) ) & digitalPinToBitMask(RELAYPIN2));
    bool relayReg3 = !(*portOutputRegister( digitalPinToPort(RELAYPIN3) ) & digitalPinToBitMask(RELAYPIN3));
    bool relayReg4 = !(*portOutputRegister( digitalPinToPort(RELAYPIN4) ) & digitalPinToBitMask(RELAYPIN4));
    bool relayReg5 = !(*portOutputRegister( digitalPinToPort(RELAYPIN5) ) & digitalPinToBitMask(RELAYPIN5));
    bool relayReg6 = !(*portOutputRegister( digitalPinToPort(RELAYPIN6) ) & digitalPinToBitMask(RELAYPIN6));
    sprintf(buffer, "Relay 1/2:   %d / %d", relayReg1, relayReg2);
    display.print(buffer);
    display.setCursor(0,36);
    sprintf(buffer, "Relay 3/4:   %d / %d", relayReg3, relayReg4);
    display.print(buffer);
    display.setCursor(0,44);
    sprintf(buffer, "Relay 5/6:   %d / %d", relayReg5, relayReg6);
    display.print(buffer);
    display.setCursor(0,54);
    //char buffer[40];
    sprintf(buffer, "uptime %02d:%02d:%02d", days, hours, minutes);    
    display.print(buffer);
    display.display();
  }
  else if (oledPage == 3){
    uint32_t heapSize = ESP.getHeapSize();
    uint32_t freeHeap = ESP.getFreeHeap(); //available heap
    uint32_t minHeap = ESP.getMinFreeHeap(); //lowest level of free heap since boot
    uint32_t maxAllocHeap = ESP.getMaxAllocHeap(); //largest block of heap that can be allocated at once
    uint8_t freeHeapPerc = freeHeap*100 / heapSize;
    uint8_t minHeapPerc = minHeap*100 / heapSize;
    
    char buffer[40];
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Heap memory (bytes):");

    display.setCursor(0,10);
    sprintf(buffer, "size:     %u", heapSize);   
    display.print(buffer); DEBUG_PRINTLN(buffer);

    display.setCursor(0,18);
    sprintf(buffer, "free:     %u %u%%", freeHeap, freeHeapPerc); 
    display.print(buffer); DEBUG_PRINTLN(buffer);

    display.setCursor(0,26); 
    sprintf(buffer, "min free: %u %u%%", minHeap, minHeapPerc); 
    display.print(buffer); DEBUG_PRINTLN(buffer);
    
    display.setCursor(0,34);
    sprintf(buffer, "max alloc %u", maxAllocHeap); 
    display.print(buffer); DEBUG_PRINTLN(buffer);

    display.display();
    
  }
}