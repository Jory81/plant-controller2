
float processRTD(uint16_t rtd){
  uint16_t ohmsx100 ;
  uint32_t dummy ;
  float ohms, Tlut ;  
  //float Tcvd, Tcube, Tpoly, Trpoly ;    

  dummy = ((uint32_t)(rtd << 1)) * 100 * ((uint32_t) floor(RREF)) ;
  dummy >>= 16 ;
  ohmsx100 = (uint16_t) (dummy & 0xFFFF) ;
  ohms = (float)(ohmsx100 / 100) + ((float)(ohmsx100 % 100) / 100.0) ; 
  Tlut  = PT100.celsius(ohmsx100); 
  return Tlut;
}

// double modifiedMap(double x, double in_min, double in_max, double out_min, double out_max)
// {
//  double temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//  temp = (int) (4*temp + .5);
//  return (double) temp/4;
// }

byte modifiedMap(double x, double in_min, double in_max, byte out_min, byte out_max)
{
 double temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 temp = (int) (4*temp + .5);
 return (double) temp/4;
}


void samplingTemp(){
  switch (systemParam.probeTypeT){
          case 1: {
            for (int sensor = 0; sensor <systemParam.probeCountT; sensor++){
              uint16_t rtd = maxthermo[sensor].readRTD();
              valid.pt100[sensor] = processRTD(rtd) - cal.pt100[sensor];              
              tempTable[sensor][readIndex] = valid.pt100[sensor];
              totalTemp[sensor]=0;
              for (int n = 0; n < systemParam.measurements; n++){
                totalTemp[sensor] = totalTemp[sensor] + tempTable[sensor][n];
              }
              valid.pt100[sensor] = totalTemp[sensor]/systemParam.measurements;
              }
              readIndex = readIndex + 1;
                if (readIndex >= systemParam.measurements) {
                readIndex = 0;
              }  
          }
          break;
          case 2: {
            // for (int sensor = 0; sensor <systemParam.probeCountT; sensor++){
            //   temp[sensor] = thermocouple[sensor]->readCelsius();
            //      if ((temp[sensor] < 300 && temp[sensor] > oldtemp[sensor]-50 && temp[sensor] < oldtemp[sensor]+50 ) || oldtemp[sensor] == 0){
            //       oldtemp[sensor] = temp[sensor] - cal.pt100sensor[sensor];;             
            //       }
            // }
          }
          break;
        }    
}

void samplingHumidity(){
    if (systemParam.probeCountH > 0){
        for (int sensor = 0; sensor < systemParam.probeCountH; sensor++){
        concept.dhtH[sensor] = dht[sensor].readHumidity();
        concept.dhtT[sensor] = dht[sensor].readTemperature();
          if(!isnan(concept.dhtT[sensor]) && !isnan(concept.dhtH[sensor])){
            valid.dhtT[sensor] = concept.dhtT[sensor] - cal.dhtT[sensor];
            valid.dhtH[sensor] = concept.dhtH[sensor] - cal.dhtH[sensor];
            // Serial.printf("sensor %d humidity ", sensor); Serial.print(humidity[sensor]);
            // Serial.printf(" temp "); Serial.println(dhtT[sensor]);
          }
          else {
            humidityCounter++;
            if (humidityCounter > 6){
              resetHumidity = true;
            }
          }  
        }
    }
}
