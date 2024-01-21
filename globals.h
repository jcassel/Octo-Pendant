//#define _OctoAPIDebug // uncomment to get debug serial messages from the Octoprint API. 

//Note that touch response times are slower when these are turned on.
//#define _debugopra // uncomment for general serial messages for core logic will also add regions for screen debugging.
//#define _debugTouch //uncomment to show touch cordenents live. 

#define CONFIG_VERSION "OctoPendant 1.0.1.8"
#define DEFAULT_HOSTS_NAME "OctoPendant-New"
#define OCTOPENDENT_API_URL "plugin/OctoPendant"

int currentScreen = 0; 
OctoprintApi api;

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

TimeRelease resetTimeDelay;
TimeRelease ReconnectWiFi;

String hmActionStatus = "Ready";
TimeRelease ClearActionStatus;

struct Wificonfig
{
  char ssid[64];
  char password[64];
  char wifimode[12];
  char hostname[64];
  int attempts;
  int attemptdelay;
  char apPassword[64];
};

Wificonfig wificonfig;
char* www_username = "admin";
ESP8266WebServer webServer(80);
WiFiClient net;
WiFiClient MQTT;

// This is the file name used to store the calibration data
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData1"
bool REPEAT_CAL = false;

struct SettingsConfig
{
  int TimeZoneOffsetHours;
  //int StatusIntervalSec;
  char unique_Id[33];
  char OPURI[128]; //octoprint URL/URI Ip or fcdn
  int OPPort; //Octoprint port
  char OPAK[34]; //octoprint API Key 32 char
  char OPLGCF[64]; //may want to adjust this if you have your lvel file deep in the OctoPrint file system.
  int OPReload; //should it reload the last object after mesh value gathering. 
  int OPStart; //Should it start the next print after mesh value gathering.
  int EML; //enable mess leveling
  int BVX; //build volumn in the X axis
  int BVY; //build volumn in the Y axis
  int BVO; //build volumn offset for manual leveling locations
  int BLAZ; //Bed level auto zero?
  int ACONP; //AutoConnect to printer Serial.
  char PProfile[34]; //Printer profile name (If blank or missing it will use _default
};
SettingsConfig settingsConfig;

int ZMAX =180;



struct MQTTConfig
{
  int MQTTEN;
  int StatusIntervalSec;
  char MQTTSrvIp[128];
  char MQTTUser[32];
  char MQTTPW[64];
  int MQTTPort;
};
MQTTConfig mqttConfig;

MQTTClient mqttClient;

unsigned long lastMqttConnectionAttempt = 0;
boolean needMqttConnect = false;
bool needReset = false;
bool doWiFiScan = false;


int defaultMotionSpeed = 3500; //1200 is ok

struct OPConnection{
  int baudrate;
  String port;
  String profileId;
  String state;
};

OPConnection currentCon;


bool motionDisabled =true;

TimeRelease ReconnectMQTT;
TimeRelease mqttStatusInerval;

TimeRelease UpdateCurrentScreen;
long UpdateCurrentScreenPollInterval = 1500ul;

TimeRelease PollPrinterStats;
long PollPrinterStatsInterval = 3050ul;

TimeRelease PollPrinterJobInfo;
long PollPrinterJobInfoInterval = 3100ul;

TimeRelease PollConnectionInfo;
long PollConnectionInfoInterval = 3000ul;

TimeRelease PollAxisInfo;
long PollAxisInfoInderval = 3010ul;

TimeRelease PollOPOnline;
long PollOPOnlineInterval = 5000ul;


bool InAPMode = false;
bool APModeSuccess = false;

uint32_t getChipId(){
  return ESP.getChipId();
}

WiFiUDP ntpUDP;
// By default 'pool.ntp.org' is used with 60 seconds update interval and no offset
//int TimeZoneOffsetHours = -5; Now in the settingsconfig struct//Eastern(US NY) standard time [This value will default to -5 but can be over ridden in the settings.d file
NTPClient timeClient(ntpUDP,"pool.ntp.org",(-5 * 3600/-1),120000);// -4 hours update every 2 minutes gets updated in setup.






String getDate()
{
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  //Serial.print("LocalTime:");
  //Serial.println(asctime(timeinfo));
  
  char timeStringBuff[25]; //25 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%F", timeinfo);
  
  return timeStringBuff;
}

void Debugwificonfig(){
  Serial.print("wificonfig.ssid: ");
  Serial.println(wificonfig.ssid);
  Serial.print("wificonfig.password: ");
  Serial.println(wificonfig.password);
  Serial.print("wificonfig.wifimode: ");
  Serial.println(wificonfig.wifimode);
  Serial.print("wificonfig.hostname: ");
  Serial.println(wificonfig.hostname);
  Serial.print("wificonfig.attempts: ");
  Serial.println(wificonfig.attempts);
  Serial.print("wificonfig.attemptdelay: ");
  Serial.println(wificonfig.attemptdelay);
  Serial.print("wificonfig.apPassword: ");
  Serial.println(wificonfig.apPassword);
}

void DebugConfigSettings(){
  

  Serial.print("settingsConfig.unique_Id: ");
  Serial.println(settingsConfig.unique_Id);
  Serial.println(settingsConfig.OPLGCF);
  Serial.println(settingsConfig.OPReload);
  Serial.println(settingsConfig.OPStart);
  Serial.println(settingsConfig.BLAZ);
  Serial.println(settingsConfig.EML);
  Serial.println(settingsConfig.PProfile);
  Serial.println(settingsConfig.ACONP);


  
  Serial.print("settingsConfig.TimeZoneOffsetHours: ");
  Serial.println(settingsConfig.TimeZoneOffsetHours);

  Serial.print("mqttConfig.MQTTEN: ");
  Serial.println(mqttConfig.MQTTEN);
  Serial.print("settingsConfig.StatusIntervalSec: ");
  Serial.println(mqttConfig.StatusIntervalSec);
  Serial.print("mqttConfig.MQTTSrvIp: ");
  Serial.println(mqttConfig.MQTTSrvIp);
  Serial.print("mqttConfig.MQTTUser: ");
  Serial.println(mqttConfig.MQTTUser);
  Serial.print("mqttConfig.MQTTPW: ");
  Serial.println(mqttConfig.MQTTPW);

}




int lastWifiStatus = -1;
bool isWiFiConnected(bool debug=true){

  if(lastWifiStatus !=WiFi.status()){ // should only print out the status when it changes.
    lastWifiStatus =WiFi.status();
    #ifdef _debugopra
      Serial.print("WiFi Status: ");
      Serial.println(WiFi.status());
    #endif
  }
  return (WiFi.status() == WL_CONNECTED);
}

bool MQTTPushStatus = true; //flag true any time you think the MQTT status should be updated.

bool WifiInitialized = false;
bool startWifiStation(){
  #ifdef _debugopra
  Serial.printf("[INFO]: Connecting to %s\n", wificonfig.ssid);
  Serial.printf("Existing set WiFi.SSID:%s\n",WiFi.SSID().c_str());
  Serial.printf("wificonfig.ssid:%s\n",wificonfig.ssid);
  #endif
  if(String(wificonfig.wifimode) == "WIFI_STA"){
    if ( String(WiFi.SSID()) != String(wificonfig.ssid) || WifiInitialized == false)
    {
        #ifdef _debugopra
        Serial.println("initiallizing WiFi Connection");
        #endif
        if(isWiFiConnected()){
          WiFi.disconnect();
        }
        
        
        WiFi.mode(WIFI_STA);
        WiFi.begin(wificonfig.ssid, wificonfig.password);
        WifiInitialized = true;
        uint8_t attempts = wificonfig.attempts;
        while (!isWiFiConnected())
        {
          if(attempts == 0) {
            WiFi.disconnect();
            Serial.println("");
            return false;
            
          }
          
          delay(wificonfig.attemptdelay);
          Serial.print(".");
          attempts--;
       }
       #ifdef _debugopra
       Serial.println(WiFi.localIP());  
       #endif
    }else{
      if(WiFi.status() != WL_CONNECTED){
        WiFi.reconnect();
      }
    }
    WiFi.hostname(wificonfig.hostname);
  }else{
    
    return false; //In AP mode.. should not be connecting.
  }
  #ifdef _debugopra
  Serial.print("WiFi.Status(): ");
  Serial.println(String(WiFi.status()));
  Serial.print("WL_CONNECTED: ");
  Serial.println(String(WL_CONNECTED));
  #endif
  needMqttConnect = (WiFi.status() == WL_CONNECTED); //get the MQtt System to connect once wifi is connected.
  return (WiFi.status() == WL_CONNECTED);
}



void DoInAPMode(){
  if(InAPMode && !APModeSuccess){
    //Going into AP mode
    #ifdef _debugopra
    Serial.println("Entering AP mode.");
    #endif
    if(wificonfig.hostname == "OctoPendant-New"){
      String tempName = wificonfig.hostname + String(getChipId()); 
      strlcpy(wificonfig.hostname,tempName.c_str(),sizeof(wificonfig.hostname));
    }
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    delay(100);
    APModeSuccess = WiFi.softAP(wificonfig.hostname, wificonfig.apPassword);
    if (APModeSuccess){
      #ifdef _debugopra
      Serial.print("SoftAP IP Address: ");
      Serial.println(WiFi.softAPIP());
      #endif
      ReconnectWiFi.set(15000); //15 seconds
    }else{
      APModeSuccess = true; // not really but if we do not do this, It will come though here and reset the reset delay every loop. 
      #ifdef _debugopra
      Serial.println("SoftAP mode Failed Rebooting in 15 seconds.");
      #endif
      resetTimeDelay.set(15000UL); //trigger 15 sec
      needReset = true;
    }
  }
}

void CheckWifi(){
   //check connection status
  if(!InAPMode || InAPMode && !APModeSuccess){
    if(!isWiFiConnected()){
      InAPMode = true; 
      DoInAPMode();
    }
    
  }else{
    //only try to reconnect if no one has connected as an AP client.
    if(ReconnectWiFi.check() && WiFi.softAPgetStationNum() ==0){
        InAPMode = !startWifiStation(); //attempt to reconnect to the main wifi access point if it succeds, Set to false.
        APModeSuccess = false; //reset so it will do a full attempt to go into AP mode if called to
    }
  }
  
  
  if(resetTimeDelay.check()){
    if(needReset){
      ESP.restart();
    }
  }
}


String IpAddress2String(const IPAddress& ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}


bool loadWiFiConfig()
{
  if (!SPIFFS.exists("/config/wificonfig.json"))
  {
    Serial.println("[WARNING]: Config file not found! Loading Factory Defaults.");
    strlcpy(wificonfig.ssid,"unset", sizeof(wificonfig.ssid));
    strlcpy(wificonfig.password, "", sizeof(wificonfig.password));
    
    strlcpy(wificonfig.wifimode, "WIFI_AP", sizeof(wificonfig.wifimode));
    strlcpy(wificonfig.hostname, DEFAULT_HOSTS_NAME, sizeof(wificonfig.hostname));
    strlcpy(wificonfig.apPassword, "Password1", sizeof(wificonfig.apPassword));
    wificonfig.attempts = 10;
    wificonfig.attemptdelay = 5000; //5 second delay
    return false;
  }
  File configfile = SPIFFS.open("/config/wificonfig.json","r");

  DynamicJsonDocument doc(256);

  DeserializationError error = deserializeJson(doc, configfile);

  strlcpy(wificonfig.ssid, doc["ssid"] | "FAILED", sizeof(wificonfig.ssid));
  strlcpy(wificonfig.password, doc["password"] | "FAILED", sizeof(wificonfig.password));
  strlcpy(wificonfig.wifimode, doc["wifimode"] | "FAILED", sizeof(wificonfig.wifimode));
  strlcpy(wificonfig.hostname, doc["hostname"] | DEFAULT_HOSTS_NAME, sizeof(wificonfig.hostname));
  strlcpy(wificonfig.apPassword, doc["apPassword"] | "Password1", sizeof(wificonfig.apPassword));

  uint8_t attempts = doc["attempts"] | 10 ;
  wificonfig.attempts = attempts;

  uint16_t attemptdelay = doc["attemptdelay"] | 5000 ;
  wificonfig.attemptdelay = attemptdelay;

  configfile.close();

  if (error)
  {
    Serial.println("[ERROR]: deserializeJson() error in loadWiFiConfig");
    Serial.println(error.c_str());
    return false;
  }

  return true;
}


bool saveWifiConfig(Wificonfig newConfig)
{

  SPIFFS.remove("/config/wificonfig.json");
  File file = SPIFFS.open("/config/wificonfig.json", "w");

  DynamicJsonDocument doc(384);

  JsonObject wificonfigobject = doc.to<JsonObject>();
  
  wificonfigobject["ssid"] = newConfig.ssid;
  wificonfigobject["password"] = newConfig.password;
  wificonfigobject["wifimode"] = newConfig.wifimode;
  wificonfigobject["hostname"] = newConfig.hostname;
  wificonfigobject["attempts"] = newConfig.attempts;
  wificonfigobject["attemptdelay"] = newConfig.attemptdelay;
  wificonfigobject["apPassword"] = newConfig.apPassword;

  if (serializeJsonPretty(doc, file) == 0)
  {
    Serial.println("[WARNING]: Failed to write to file");
    return false;
  }
  file.close();
  return true;
}

bool saveSettingsConfig(SettingsConfig newConfig){
  SPIFFS.remove("/config/settings.json");
  File file = SPIFFS.open("/config/settings.json", "w");

  DynamicJsonDocument doc(544);

  JsonObject settingsconfigobject = doc.to<JsonObject>();
  
  settingsconfigobject["unique_Id"]= newConfig.unique_Id;
  //settingsconfigobject["StatusIntervalSec"]= newConfig.StatusIntervalSec;
  settingsconfigobject["TimeZoneOffsetHours"]= newConfig.TimeZoneOffsetHours;
  //settingsconfigobject["MQTTSrvIp"]= newConfig.MQTTSrvIp;
  //settingsconfigobject["MQTTUser"]= newConfig.MQTTUser;
  //settingsconfigobject["MQTTPW"]= newConfig.MQTTPW;
  //settingsconfigobject["MQTTPort"]= newConfig.MQTTPort;

  settingsconfigobject["OPURI"]= newConfig.OPURI;
  settingsconfigobject["OPAK"]= newConfig.OPAK;
  settingsconfigobject["OPPort"]= newConfig.OPPort;
  settingsconfigobject["OPLGCF"]= newConfig.OPLGCF;
  
  settingsconfigobject["OPReload"]= newConfig.OPReload;
  settingsconfigobject["OPStart"]= newConfig.OPStart;
  settingsconfigobject["BLAZ"]= newConfig.BLAZ; 
  settingsconfigobject["EML"]= newConfig.EML; 
  
  settingsconfigobject["BVX"]= newConfig.BVX;
  settingsconfigobject["BVY"]= newConfig.BVY;
  settingsconfigobject["BVO"]= newConfig.BVO; 

  settingsconfigobject["ACONP"]= newConfig.ACONP; 
  settingsconfigobject["PProfile"]= newConfig.PProfile; 
  
  
  
  if (serializeJsonPretty(doc, file) == 0)
  {
    Serial.println("[WARNING]: Failed to write to file:/config/settings.json");
    return false;
  }
  file.close();
  return true;
}


bool saveMQTTConfig(MQTTConfig newConfig){
  SPIFFS.remove("/config/mqttconfig.json");
  File file = SPIFFS.open("/config/mqttconfig.json", "w");

  DynamicJsonDocument doc(544);

  JsonObject mqttConfigObject = doc.to<JsonObject>();
  
  mqttConfigObject["MQTTEN"]= newConfig.MQTTEN;
  mqttConfigObject["StatusIntervalSec"]= newConfig.StatusIntervalSec;
  mqttConfigObject["MQTTSrvIp"]= newConfig.MQTTSrvIp;
  mqttConfigObject["MQTTUser"]= newConfig.MQTTUser;
  mqttConfigObject["MQTTPW"]= newConfig.MQTTPW;
  mqttConfigObject["MQTTPort"]= newConfig.MQTTPort;

  if (serializeJsonPretty(doc, file) == 0)
  {
    Serial.println("[WARNING]: Failed to write to file:/config/mqttconfig.json");
    return false;
  }
  file.close();
  return true;

}


bool loadMQTTConfig(){
  if(!SPIFFS.exists("/config/mqttconfig.json")){
    #ifdef _debugopra
    Serial.println("[WARNING]: Config [/config/mqttconfig.json] file not found!");
    #endif
    return false;
  }else{
    #ifdef _debugopra
    Serial.println("Loading /config/mqttconfig.json");
    #endif
  }

  File configfile = SPIFFS.open("/config/mqttconfig.json","r");
  DynamicJsonDocument doc(544);
  DeserializationError error = deserializeJson(doc, configfile);

  int mqttEn = doc["MQTTEN"];
  mqttConfig.MQTTEN = mqttEn;
  #ifdef _debugopra
  Serial.println("mqttConfig.MQTTENTPort =" + String(mqttConfig.MQTTEN));
  #endif
  
  int StatusIntervalSec = doc["StatusIntervalSec"];
  mqttConfig.StatusIntervalSec = StatusIntervalSec;
  #ifdef _debugopra
  Serial.println("mqttConfig.StatusIntervalSec =" + String(StatusIntervalSec));
  #endif 

  
  strlcpy(mqttConfig.MQTTSrvIp, doc["MQTTSrvIp"] | "Unknown", sizeof(mqttConfig.MQTTSrvIp));
  strlcpy(mqttConfig.MQTTUser, doc["MQTTUser"] | "Unknown", sizeof(mqttConfig.MQTTUser));
  strlcpy(mqttConfig.MQTTPW, doc["MQTTPW"] | "Unknown", sizeof(mqttConfig.MQTTPW));

  int mqttport = doc["MQTTPort"];
  mqttConfig.MQTTPort = mqttport;
  #ifdef _debugopra
  Serial.println("mqttConfig.MQTTPort =" + String(mqttport));
  #endif
    configfile.close();

  if (error)
  {
    Serial.println("[ERROR]: deserializeJson() error in loadSettingsConfig");
    Serial.println(error.c_str());
    return false;
  }

  return true;
  
}



bool loadSettingsConfig(){

  if(!SPIFFS.exists("/config/settings.json")){
    #ifdef _debugopra
    Serial.println("[WARNING]: Config [/config/settings.json] file not found!");
    #endif
    return false;
  }else{
    #ifdef _debugopra
    Serial.println("Loading /config/settings.json");
    #endif
  }
  
  File configfile = SPIFFS.open("/config/settings.json","r");
  DynamicJsonDocument doc(544);
  DeserializationError error = deserializeJson(doc, configfile);

  strlcpy(settingsConfig.unique_Id, doc["unique_Id"] | "Unknown", sizeof(settingsConfig.unique_Id));
  
  strlcpy(settingsConfig.OPURI, doc["OPURI"] | "Unknown", sizeof(settingsConfig.OPURI));
  strlcpy(settingsConfig.OPAK, doc["OPAK"] | "Unknown", sizeof(settingsConfig.OPAK));
  strlcpy(settingsConfig.OPLGCF, doc["OPLGCF"] | "", sizeof(settingsConfig.OPLGCF));
  strlcpy(settingsConfig.PProfile, doc["PProfile"] | "_default", sizeof(settingsConfig.PProfile));
  
  
  int OPPort = doc["OPPort"];
  settingsConfig.OPPort = OPPort;

  int OPReload = doc["OPReload"];
  settingsConfig.OPReload = OPReload;
  
  int OPStart = doc["OPStart"];
  settingsConfig.OPStart = OPStart;

  int BLAZ = doc["BLAZ"];
  settingsConfig.BLAZ = BLAZ;
  
  int EML = doc["EML"];
  settingsConfig.EML = EML;


  int BVX = doc["BVX"];
  settingsConfig.BVX = BVX;

  int BVY = doc["BVY"];
  settingsConfig.BVY = BVY;

  int BVO = doc["BVO"];
  settingsConfig.BVO = BVO;

  int ACONP = doc["ACONP"];
  settingsConfig.ACONP = ACONP;
  
  
  

  int TimeZoneOffsetHours = doc["TimeZoneOffsetHours"];
  settingsConfig.TimeZoneOffsetHours = TimeZoneOffsetHours;
  #ifdef _debugopra
  Serial.println("settingsConfig.TimeZoneOffsetHours = " + String(TimeZoneOffsetHours));
  #endif
  configfile.close();

  if (error)
  {
    Serial.println("[ERROR]: deserializeJson() error in loadSettingsConfig");
    Serial.println(error.c_str());
    return false;
  }

  return true;



}


void SetupWifi(){
  if(loadWiFiConfig()){
    #ifdef _debugopra
    Serial.println("WiFi Config Loaded");
    #endif
    InAPMode  = false;
  }else{
    InAPMode  = true;
  }
  if(InAPMode){
    DoInAPMode();
  }else{
    startWifiStation();
  }
  #ifdef _debugopra
  Serial.println("Wifi Setup complete");
  #endif
}

/* --------------- Checking for free space on SPIFFS ---------------- 
Purpose: This checks if the free memory on the SPIFFS is bigger then a set threshold
Input  : none
Output : boolean
Note   : none
*/
float SpaceLeft(){
  FSInfo fs_info;
  
  float freeMemory = fs_info.totalBytes - fs_info.usedBytes;
  return freeMemory;
  
}

bool IsSpaceLeft()
{
  float minmem = 100000.00; // Always leave 100 kB free pace on SPIFFS
  float freeMemory = SpaceLeft();
  Serial.printf("[INFO]: Free memory left: %f bytes\n", freeMemory);
  if (freeMemory < minmem)
  {
    return false;
  }

  return true;
}


void InitStorageSetup(){
  SPIFFS.begin();
  //do anything that might be needed related to file and storage on startup.
}
