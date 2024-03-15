#include <Arduino.h>

#ifdef ESP8266
 #include <ESP8266WiFi.h>
 #include "FS.h" 
#else
 #include <WiFi.h>
 #include <SPIFFS.h>
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#include <ESPAsyncWebServer.h>

#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>
#include <Adafruit_MAX31865.h>

#if (defined(__AVR__))
#include <avr\pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <pt100rtd.h>
#include <AutoPID.h>

#include <DHT.h>

#include "RTClib.h"
#include "time.h"

//#define DEBUG_OUTPUT // comment out for debugging mode (mainly for checking memory issues and JSON communication)

#ifdef DEBUG_OUTPUT
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(x...) Serial.printf(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(x...)
#endif



RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const char* ntpServer = "pool.ntp.org";

// OLED SD1306 properties
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//#define RELAY_PIN   17

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// THERMOCOUPLE PINS
#define SCK_PIN 18
#define CS_PIN 5
#define SO_PIN 19
#define CS2_PIN 22
// #define SO_PIN 21
// #define CS2_PIN 26
// NOT FULLY TESTED AND OPERATIONAL FOR MULTIPLE SENSORS - FYI

const int RELAYPIN1 = 14;
const int RELAYPIN2 = 13;
const int RELAYPIN3 = 25; 
const int RELAYPIN4 = 33;
const int RELAYPIN5 = 2; // DISCONNECT PIN AT FLASH - CONNECT AFTER FLASHING
const int RELAYPIN6 = 15; //

#define OUTPUT_PIN1 4 // Fan1
const int freq = 10;
const int ledChannel1 = 0;
const int resolution = 8;

#define OUTPUT_PIN2 0 // fan2 not tested
//const int freq = 10;
const int ledChannel2 = 1;

Thermocouple* thermocouple[5];
//Thermocouple* thermocouple2;

#define updateTempAndRelays 1000 //1000
#define updateHumidity 5000
#define updateOledDisplay 5000
#define updateFans 1000

#define DHTPIN1 16 // 
#define DHTPIN2 17
#define DHTPIN3 12 // DISCONNECT PIN AT FLASH - CONNECT AFTER FLASHING/ RUNNING 

DHT dht[] = {
  {DHTPIN1, DHT22},
  {DHTPIN2, DHT22},
  {DHTPIN3, DHT22},
};

Adafruit_MAX31865 maxthermo[4] = {Adafruit_MAX31865(5), Adafruit_MAX31865(26), Adafruit_MAX31865(27), Adafruit_MAX31865(32)} ; // 5, 26, 27, 32, 12 // 35 probably won't work, but there are no pins left
//Adafruit_MAX31865 maxthermo[5] = {Adafruit_MAX31865(34), Adafruit_MAX31865(35), Adafruit_MAX31865(36), Adafruit_MAX31865(39), Adafruit_MAX31865(5)} ;
// The value of the Rref resistor. Use 430.0!
#define RREF 430.0

// Like, duh.
#define C2F(c) ((9 * c / 5) + 32)

pt100rtd PT100 = pt100rtd();

/*
 * functions used in this script
 */
float processRTD(uint16_t rtd);
void initializeEEPROMvariables();
void timeControl();
void syncTimeRTC();
void reInitializeTimeStrings();
void reInitializeTimeInts();
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
int convertStringToInt(String str, int workflow);
//double modifiedMap(double x, double in_min, double in_max, double out_min, double out_max);
byte modifiedMap(double x, double in_min, double in_max, byte out_min, byte out_max);

void sendTempToClient();
void sendHumidityToClient();
void samplingTemp();
void samplingHumidity();
void updateGraph();

void displayOledScreen(float temp1, float temp2, float temp3, float temp4);

void fan1Control();
void light1Control(boolean manualRelay, const int RELAYPIN);
void heater1Control(boolean manualRelay, const int RELAYPIN);
void humidity1Control(boolean manualRelay, const int RELAYPIN);
void messageFanState1();

void executeTask(byte function, boolean manualRelay, const int RELAYPIN);

void fan2Control();
void light2Control(boolean manualRelay, const int RELAYPIN);
void heater2Control(boolean manualRelay, const int RELAYPIN);
void humidity2Control(boolean manualRelay, const int RELAYPIN);
void messageFanState2();

void light3Control(boolean manualRelay, const int RELAYPIN);
void heater3Control(boolean manualRelay, const int RELAYPIN);
void heater4Control(boolean manualRelay, const int RELAYPIN);
void humidity3Control(boolean manualRelay, const int RELAYPIN);

void noFunction(boolean manualRelay, const int RELAYPIN);
void DHTsensors(boolean manualRelay, const int RELAYPIN);

void onRootRequest(AsyncWebServerRequest *request);
void initWebServer();
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void initWebSocket();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void notifyClients(); 
void notifyClientsSingleObject(String object, boolean value);
void notifyClientsSingleObjectByte(String object, byte value);
void notifyClientsSingleString(String object, String &message);
void sendProgramInfo();

void setupESP32();
void setupOledScreen();
void setupSPIFFS();
void setupEEPROM();
void setupWIFI();
void setupRTC();
void setupTempSensors();
void setupDHTSensors();
void setupFans();
void setupRelays();

#include "globalVariables.h"

//input/output variables passed by reference, so they are updated automatically
// AutoPID myPID(&temperature, &setPoint, &outputVal1, OUTPUT_MIN1, OUTPUT_MAX1, KP, KI, KD);

//#include "readSensors.h"
#include "setupFunctions.h"
#include "websocketMessages.h"
#include "oledDisplay.h"
#include "relayFunctions.h"
#include "readSensors.h"

//#include <OneWire.h>

void setup()
{
setupESP32();
setupOledScreen();
setupSPIFFS();
setupEEPROM();
setupWIFI();
setupRTC();
setupTempSensors();
setupDHTSensors();
setupFans();
setupRelays();

// myPID.setBangBang(tempRange1);
// myPID.setTimeStep(100);
}

void loop(){
    //ws.cleanupClients();

    if (millis() - previousMillis1 >= systemParam.tempUpdate){
      timeControl();
      samplingTemp();        
      sendTempToClient();
      if (relay1.connected){executeTask(relay1.function, relay1.manual, RELAYPIN1);};
      if (relay2.connected){executeTask(relay2.function, relay2.manual, RELAYPIN2);}; 
      if (relay3.connected){executeTask(relay3.function, relay3.manual, RELAYPIN3);};
      if (relay4.connected){executeTask(relay4.function, relay4.manual, RELAYPIN4);};
      if (relay5.connected){executeTask(relay5.function, relay5.manual, RELAYPIN5);};
      if (relay6.connected){executeTask(relay6.function, relay6.manual, RELAYPIN6);};
    previousMillis1 = millis();     
    }

    if (millis() - previousMillis2 >= updateHumidity){
      samplingHumidity();
      sendHumidityToClient();
    previousMillis2 = millis();
    }

    if (millis() - previousMillis3 >= systemParam.graphUpdate){
      updateGraph();
    previousMillis3 = millis();
    }

    if (millis() - previousMillis4 >= updateOledDisplay){
      displayOledScreen(valid.pt100[0], valid.pt100[1], valid.pt100[2], valid.pt100[3]);
    previousMillis4 = millis();
    }
    
    if (millis() - previousMillis5 >= updateFans){
      if (fan1.connected){
        fan1Control();
        }
      if (fan2.connected){
        fan2Control();
        }
      previousMillis5 = millis();
    }
}

void timeControl(){
  currentMillis = millis();
  seconds = currentMillis / 1000;
  minutes = (seconds / 60);
  hours = (minutes / 60);
  days = hours / 24;

  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  if (systemParam.externalRTC){
    DateTime now = rtc.now(); // see syncTimeRTC
    dateHour = now.hour();
    dateMinute = now.minute();
    currentMinutes = ((dateHour*60)+dateMinute);
  }

  else if (!systemParam.externalRTC) { // Maybe there's still a bug in this code - before testing it thoroughly I included an RTC - So the 'hour' might be off by 1.
    uint8_t mismatch;
    if (minutes + minutesStart > 59){
        mismatch = 1;
        }
    else {
        mismatch = 0;
        }

    dateMinute = minutes + minutesStart;
    dateHour = hours + hourStart + mismatch;
    dateMinute %= 60;
    dateHour %= 24;
    currentMinutes = ((dateHour*60)+dateMinute);
  }
}