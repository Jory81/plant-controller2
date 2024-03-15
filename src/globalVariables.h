struct tempSettings
{
  float targetSoilTemp;
  float daySoilTemp;
  float nightSoilTemp;
  float tempRange;
  float alarmRange;
  boolean soilAlarm;
  float targetAirTemp;
  boolean airAlarm;
};
tempSettings tempParam1 {27.0f, 27.0f, 25.0f, 0.5f, 5.0f, false, 24.0f, false}; // peppers
tempSettings tempParam2 {24.0f, 24.0f, 22.0f, 0.5f, 5.0f, false, 26.0f, false}; // tomatoes
tempSettings tempParam3 {28.0f, 27.0f, 22.0f, 0.5f, 5.0f, false, 26.0f, false}; // seedlings
tempSettings tempParam4 {28.0f, 27.0f, 22.0f, 0.5f, 5.0f, false, 26.0f, false}; // seedlings

struct Relay
{
boolean connected;
boolean manual;
byte function;
};
Relay relay1 {false, false, 0}, relay2 {false, false, 0}, relay3 {false, false, 0}, relay4 {false, false, 0}, relay5 {false, false, 0}, relay6 {false, false, 0};

struct Fan
{
boolean connected;
boolean manual;
boolean daySwitch;
byte manualSpeed;
byte speed;
byte oldSpeed;
byte output_min;
byte output_max;
boolean fanState;
};
Fan fan1 {false, false, true, 0, 0, 0, 165, 255, false};
Fan fan2 {false, false, true, 0, 0, 0, 165, 255, false};

struct humidSettings
{
byte humidmin;
byte humidmax;
};
humidSettings humidParam1 {30, 80}, humidParam2 {30, 80};

struct Climate
{
boolean heater;
boolean humidifier;
boolean fan;
boolean lights;
};
Climate climate1, climate2, climate3, climate4;

struct Sensors
{
float pt100[4];
float dhtT[2];
float dhtH[2];
};
Sensors concept, valid, cal {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

struct TimeKeeping
{
int hoursOn = 0;
int minutesOn = 0;
int hoursOff = 0;
int minutesOff = 0;
int minutesLightsOn = 0;
int minutesLightsOff = 0;
String lightsON;
String lightsOFF;

struct lightSettings
{
char lightsOn[6];
char lightsOff[6];
} lights;

};
TimeKeeping schedule1, schedule2, schedule3;


struct SystemSettings{
uint8_t probeTypeT; //uint8_t sensorType = 1;
uint8_t probeCountT; // uint8_t sensorAmount = 1;
uint8_t probeTypeH; 
uint8_t probeCountH; // uint8_t humiditySensorAmount = 0;
byte measurements;   
boolean externalRTC;
boolean resetRTC;
uint32_t graphUpdate; //uint32_t updateTimeGraph = 5000;
uint32_t tempUpdate; //uint32_t updateTimeGraph = 5000;
char SSID[32]; // or char[100]; //!
char PASS[32]; // or char[100]; //!
};

SystemSettings systemParam{
1, 1, 1, 0, 1, 0, 0, 5000, 1000,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

struct storeEEPROM{
int check; 
SystemSettings systemParam;
tempSettings tempParam1, tempParam2, tempParam3, tempParam4;
humidSettings humidParam1, humidParam2;
Relay relay1, relay2, relay3, relay4, relay5, relay6;
Fan fan1, fan2; 
TimeKeeping::lightSettings lights1, lights2, lights3;
Sensors cal;
};

storeEEPROM myVar{
10001, // check
1, 1, 1, 0, 1, 0, 1, 5000, 1000, // systemParameters
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // SSID
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // PASS
27.0f, 27.0f, 25.0f, 0.5f, 5.0f, false, 25.0f, false, // Temp parameters climate 1
24.0f, 24.0f, 22.0f, 0.5f, 5.0f, false, 25.0f, false, // Temp parameters climate 2
28.0f, 27.0f, 22.0f, 0.5f, 5.0f, false, 25.0f, false, // Temp parameters climate 3
28.0f, 27.0f, 22.0f, 0.5f, 5.0f, false, 25.0f, false, // Temp parameters climate 4
30, 80, // humid parameters climate 1
30, 80, // humid parameters climate 2
false, false, 0, // relay settings relay 1
false, false, 0, // relay settings relay 2
false, false, 0, // relay settings relay 3
false, false, 0, // relay settings relay 4
false, false, 0, // relay settings relay 5
false, false, 0, // relay settings relay 6
false, false, true, 0, 0, 0, 165, 255, false, // fan settings climate 1
false, false, true, 0, 0, 0, 165, 255, false, // fan settings climate 2
0, 0, 0, 0, 0, 0, // lights1 ON
0, 0, 0, 0, 0, 0, // lights1 OFF
0, 0, 0, 0, 0, 0, // lights2 ON
0, 0, 0, 0, 0, 0, // lights2 OFF
0, 0, 0, 0, 0, 0, // lights3 ON
0, 0, 0, 0, 0, 0, // lights3 OFF
0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f // cal (calibration settings)
};

char stringStorage[32];

float tempTable[4][16] = { 
  {25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, },
  {25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, },
  {25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, },
  {25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, 25.0f, }
};
int readIndex = 0;
// byte measurements = 1;             

float totalTemp[4] = {0, 0, 0, 0};                  

boolean Reboot = false;
boolean saveInEEPROM = false;

const char *WIFI_SSID_AP = "ESP32-AP";
const char *WIFI_PASS_AP =  "";
String wifiID;
String wifiPASS;

boolean wifiStationMode=false;

boolean resetHumidity = false;
boolean sensorsOff = false;
int humidityCounter = 0;
int resetCount = 0;
//TIME RELATED PARAMETERS
unsigned long previousMillis=0;
unsigned long previousMillis1=0;
unsigned long previousMillis2=0;
unsigned long previousMillis3=0;
unsigned long previousMillis4=0;
unsigned long previousMillis5=0;

unsigned long currentMillis = 0;
unsigned long seconds = 0;
//int seconds = 0;
int minutes = 0;
int hours = 0;
int days = 0;

int dateHour = 0;
int dateMinute = 0;
int currentMinutes = 0;

// int totalMinutes = 0;
// int totalHours = 0;
int minutesStart = 0;
int hourStart = 0;

uint16_t EEPROMposition = 0;
uint16_t stringLength;
boolean msgFanState1= true;
boolean msgFanState2= true;
byte fanspeed1 = 0;
byte fanspeed2 = 0;
boolean displayTime;
uint8_t oledPage = 0;

#ifdef DEBUG_OUTPUT
uint8_t oledMaxPage = 3;
#else
uint8_t oledMaxPage = 2;
#endif
