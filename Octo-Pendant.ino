#include <ArduinoJson.h> 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Bounce2.h>
#include <NTPClient.h>
#include <MQTT.h>
#include "TimeRelease.h"
#include "ActionRegion.h"
#include <OctoPrintAPI.h> //would be best to use this fork: https://github.com/jcassel/OctoPrintAPI but the parent at https://github.com/chunkysteveo/OctoPrintAPI should work as well. 
#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
#include "globals.h"
#include "MQTTControl.h"
#include <ESP8266WebOTA.h> //Lib is here: https://github.com/jcassel/ESP8266WebOTA
#include "FS.h"
#define FS_NO_GLOBALS
#include "webPages.h"

#include <Time.h>         //We will need these two just to do some rough time math on the timestamps we get
#include <TimeLib.h>

void setup() {
  Serial.begin(74880);
  #ifdef _debugopra
  Serial.println("Begin Setup");
  
  struct  rst_info  *rtc_info = system_get_rst_info();
  os_printf("reset  reason: %x\n",  rtc_info->reason);
  Serial.println(rtc_info->reason);
   if (rtc_info->reason ==  REASON_WDT_RST  || rtc_info->reason ==  REASON_EXCEPTION_RST  || rtc_info->reason ==  REASON_SOFT_WDT_RST)  {
     if (rtc_info->reason ==  REASON_EXCEPTION_RST) {
       os_printf("Fatal exception (%d):\n", rtc_info->exccause);
      }
      //The address of  the last  crash is  printed,  which is  used  to debug garbled output.
     os_printf("epc1=0x%08x,  epc2=0x%08x,  epc3=0x%08x,  excvaddr=0x%08x,depc=0x%08x\n",rtc_info->epc1,  rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr,  rtc_info->depc);
   }
  #endif
  
  #ifdef _OctoAPIDebug
  api._debug = true;
  #endif
   
  InitStorageSetup(); //start spiffs
  SetupWifi();
  
  loadSettingsConfig();
  loadMQTTConfig();
  
  initialisePages();
  webServer.begin();
  
  ESP8266webota.init(&webServer,"/update","admin",wificonfig.apPassword);
  ESP8266webota.filePathOverRide = "/ota.htm";


  if(mqttConfig.MQTTEN){
    ReconnectMQTT.set(10000UL); //check in 10sec on startup.
    mqttClient.begin(mqttConfig.MQTTSrvIp, MQTT);
    mqttClient.onMessage(mqttMessageReceived);
    mqttStatusInerval.set(mqttConfig.StatusIntervalSec * 1000);
  }

  api.init(net, settingsConfig.OPURI, settingsConfig.OPPort, settingsConfig.OPAK);
  tft.init();
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);    // HIGH to Turn on; 
  #ifdef _debugopra
  Serial.println("tft.init():complete");
  #endif
  
  // Set the rotation before we calibrate
  tft.setRotation(0);
  #ifdef _debugopra
  Serial.println("tft.setRotation(2):complete");
  #endif
  
  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();
  #ifdef _debugopra
  Serial.println("touch_calibrate():complete");
  #endif
  
  initHomeScreen();

  //set min polling time these are low so that they get run first time quickly.
  PollPrinterStats.set(100ul);
  PollPrinterJobInfo.set(100ul);
  UpdateCurrentScreen.set(10ul);
  PollConnectionInfo.set(30ul);
  PollAxisInfo.set(20ul);
  PollOPOnline.set(500ul);
  delay(100);

}

void loop() {
  CheckWifi();
  if(mqttConfig.MQTTEN){
    if(mqttStatusInerval.check()){
      MQTTPushStatus = true;
      mqttStatusInerval.set(mqttConfig.StatusIntervalSec * 1000);
    }
  
    if(!InAPMode && CheckMQTTConnection()){
      mqttClient.loop();
      if(MQTTPushStatus == true){
        MQTTPushStatus = false;
        MQTTStatus();
      }
    }
  }

  webServer.handleClient();
  doPrinterAutoConnect();
  CheckSerialCommands();
  
  CheckTouch();

  if(UpdateCurrentScreen.check()){
    ProcessScreenData();
    UpdateCurrentScreen.set(UpdateCurrentScreenPollInterval); //set interval for next update of screen.
  }
  
  if(doWiFiScan){
    doWiFiScan = false;
    int n = WiFi.scanNetworks();
    storeWifiScanResult(n);
  }
  
  isOctoPrintOnline(); //not really doing much but will populate the version info struct

  
} //end loop

void doPrinterAutoConnect(){
  if(settingsConfig.ACONP){
    if(apigetConnectionInfo()){
      if(currentCon.state != "Operational"){
        //Serial.println("Printer connection is not Operational");
        #ifdef _debugopra
        Serial.println(currentCon.state);
        #endif
        if(currentCon.state != "Opening serial connection" || currentCon.state != "Connecton request sent"){
          //Try to reconnect to settingsConfig.PProfile
          String command = "{\"command\":\"connect\",\"autoconnect\": true,\"printerProfile\": \""+ String(settingsConfig.PProfile) + "\",\"port\":\"COM10\"}";
          api.sendPostToOctoPrint("/api/connection",command.c_str());
          currentCon.state = "Connecton request sent"; //Set this so that we dont spam Ocoprint with connection commands. It will update with the next status request.
          //should set check interval to maybe 1 sec. for a few iterations. 
        }
      }
    }
  }
}



bool CheckTouch(){
  // Pressed will be set true is there is a valid touch on the screen
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  
  bool pressed = tft.getTouch(&t_x, &t_y);
  
  if(pressed){
    //Serial.println("pressed");
  }

  
  bool wasPushed = false;
  if(currentScreen == 0){
    wasPushed = hmRgnPushed(t_x,t_y);
  }else if(currentScreen == 1){
    wasPushed = ctlRgnPushed(t_x,t_y);
    //wasPushed = ActionRegionPushed(t_x,t_y,ctlscrnRgns,ctlscrnRgnsSize);
  }else if (currentScreen == 2){
    wasPushed = tmpRgnPushed(t_x,t_y)  ;
  }else if (currentScreen == 3){
    wasPushed = lvlRgnPushed(t_x,t_y);
  }else if (currentScreen == 4){
    //ActionRegionPushed(t_x,t_y,infoScrnARgn,infoScrnRgnSize);
    wasPushed = infoRgnPushed(t_x,t_y);
  }else if(currentScreen == 5){
    wasPushed = CtlMnuRgnPushed(t_x,t_y);
  }else if(currentScreen == 6){
    wasPushed = filCtlRgnPushed(t_x,t_y);
  }
  
  
  if(t_x != 0 ||t_y != 0){

    #ifdef _debugTouch
    
    //Serial.print("Pressure = ");
    //Serial.print(p.z);
    Serial.print("x = ");
    Serial.print(t_x);
    Serial.print(", y = ");
    Serial.print(t_y);
    Serial.println();
    #endif
  }

  return wasPushed;
  
  
}


void ProcessScreenData(){
  
  if(currentScreen ==0){
    setHomeText(false);
  } else if (currentScreen == 1){
    setControlScreenText(); 
  }else if (currentScreen == 2){
    setTemperatureScreenText();
  }else if (currentScreen == 3){
    setLevelScreenText();
  }else if (currentScreen == 4){
    setInfoScreenText();
  }else if (currentScreen == 5){
    setCtlMnuScreenText();
  }else if (currentScreen == 6){
    setfilCtlScreenText();
  }

  //checkon background Processes
  checkRunningMeshCollection(); //dont care what screen you are on.. This needs to continue to work after you leave the screen.
  
}




void PrintNetStat(){
  Serial.print("Is Wifi Connected: ");
  Serial.println();
  if (isWiFiConnected(false)){
    Serial.println("yes");
    Serial.print("IP:");
    Serial.println(WiFi.localIP());
    
    Serial.print("SubNet:");
    Serial.println(WiFi.subnetMask());

    Serial.print("Gateway:");
    Serial.println(WiFi.gatewayIP());
    //would like to add other info here. Things like gateway and dns server and Maybe MQTT info
  }else{
    Serial.println("No");
  }
  
}

void CheckSerialCommands(){
  if (Serial.available())
  {
    String command = Serial.readStringUntil(' ');
    String value = Serial.readString();
    
    if (command == "help"){
      Serial.println("Avalible commands: (all commands should end with a space when there are no parameters)");
      Serial.println("setssid [param]; sets the wifi SSID to the param value");
      Serial.println("setpassword [param]; sets the wifi password to the param value");
      Serial.println("setwifimode [param]; sets the wifi Mode for next startup to the param value");
      Serial.println("sethostname [param]; sets the wifi password to the param value");
      Serial.println("setappassword [param]; sets the wifi password to the param value");
      Serial.println("setattempts [param]; sets the wifi password to the param value");
      Serial.println("setattemptdelay [param]; sets the wifi password to the param value");
      Serial.println("savesettings ; applys any values updates to permenent settings");
      Serial.println("restart ; restarts the pendant instantly");
      Serial.println("netstat ; returns network info about the wifi connection");
      Serial.println("tcal ; begins a touch screen calabration");
      Serial.println("settooltemp [param]; sends command to Octoprint to set hot end temperature to param value (number in deg c)");
      Serial.println("setbedtemp [param]; sends command to Octoprint to set Bed temperature to param value (number in deg c)");
      Serial.println("setpoll [param]; sets interval to poll the Octoprint instance for status (number in ms) ");
      #ifdef _debugopra
      Serial.println("font [param]; Sets font number for system display [experemental could be fatal]");
      Serial.println("getsrgn [param]; gets the current screens region info for the supplied param(number)");
      #endif
    }
    else if (command == "setssid"){strlcpy(wificonfig.ssid,value.c_str(),sizeof(wificonfig.ssid));Serial.println("SSID Set");}
    else if (command == "setpassword"){strlcpy(wificonfig.password,value.c_str(),sizeof(wificonfig.password));Serial.println("Password Set");}
    else if (command == "setwifimode"){strlcpy(wificonfig.wifimode,value.c_str(),sizeof(wificonfig.wifimode));Serial.println("WiFiMode Set");}
    else if (command == "sethostname"){ strlcpy(wificonfig.hostname,value.c_str(),sizeof(wificonfig.hostname));Serial.println("HostName Set");}
    else if (command == "setappassword"){strlcpy(wificonfig.apPassword,value.c_str(), sizeof(wificonfig.apPassword));Serial.println("AP Password Set");}
    else if (command == "setattempts"){wificonfig.attempts = value.toInt();Serial.println("Number of connection attempts Set");}
    else if (command == "setattemptdelay"){wificonfig.attemptdelay = value.toInt();Serial.println("Connection Attempt delay Set");}
    else if (command == "savesettings"){saveWifiConfig(wificonfig);Serial.println("Configuration saved. Restart to apply.");}
    else if (command == "restart" || command == "reset" || command == "reboot"){Serial.println("[WARNING]: Restarting");ESP.restart();}
    else if (command == "netstat"){PrintNetStat();}
    else if (command == "RunMeshCollection"){if (value=="true" || value=="1"){RunMeshCollection(true);}else{RunMeshCollection(false);}}
    else if (command == "tcal"){REPEAT_CAL=true; touch_calibrate(); initControlScreen();}
    else if (command == "settooltemp"){api.octoPrintSetTool0Temperature(value.toInt());}
    else if (command == "setbedtemp"){api.octoPrintSetBedTemperature(value.toInt());}
    else if (command == "setpoll"){UpdateCurrentScreenPollInterval = value.toInt();}
    else if (command == "spiffstat"){Serial.print("Space Left Bytes:");Serial.println(SpaceLeft());}
    
  }
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    #ifdef _debugopra
    Serial.println("Formating file system");
    #endif
    
    SPIFFS.format();
    SPIFFS.begin();
    
    #ifdef _debugopra
    Serial.println("SPIFFs Ready");
    #endif
  }else{
    #ifdef _debugopra
    Serial.println("SPIFFs Ready");
    #endif
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL )
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }else{
    #ifdef _debugopra
    Serial.println("Calabration File Exists");
    #endif
  }

  if (calDataOK && !REPEAT_CAL) {
    #ifdef _debugopra
    Serial.println("Cal Data Valid");
    #endif
    // calibration data valid
    tft.setTouch(calData);
  } else {
    #ifdef _debugopra
    Serial.println("Cal data not valid so recalibrate");
    #endif
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}


/*
 * This is to create a way to throttle the requests 
 * Starting at 3 sec. seems to work well. But in some 
 * cases you may want to make the direct call and 
 * reset the runningMillis if you want to show 
 * something happened with an imediate update.
 */

String previousPath;
bool runningMeshCollection;
bool mcUseSettings = false;


bool apigetPrintJob(){
    if(PollPrinterJobInfo.check()){
      api.getPrintJob();
      #ifdef _debugopra
      Serial.println("Print Job info updated");
      #endif
      PollPrinterJobInfo.set(PollPrinterJobInfoInterval);
      return true;
    }
  return false;
}

bool apigetPrintStats(){
  if(PollPrinterStats.check()){
    api.getPrinterStatistics();
    #ifdef _debugopra
    Serial.println("Printer Stats updated");
    #endif
    PollPrinterStats.set(PollPrinterStatsInterval);
    return true;
  }
  return false;
}

bool apigetConnectionInfo(){
  if(PollConnectionInfo.check()){
    #ifdef _debugopra
    Serial.println("Printer Connection info updated");
    #endif
    PollConnectionInfo.set(PollConnectionInfoInterval);
    return getPrinterConnectionInfo();
  }
  return false;
}

//  this is here and not in the Level Screen purposfully. Its something that runs
//  even if you leave the screen and needs to be monitored for completion. 
void checkRunningMeshCollection(){
  if(runningMeshCollection){
    if (apigetPrintJob() && api.printJob.printerState != "Printing"){ //only check when we get an update of info.
      #ifdef _debugopra
        Serial.println("meshCollection is done running");  
      #endif
      if(mcUseSettings && settingsConfig.OPReload){
        api.octoPrintFileSelect("/"+previousPath);
        if(settingsConfig.OPStart){
          api.octoPrintJobStart();
        }
        
      }
      runningMeshCollection = false;
    }else{
      #ifdef _debugopra
        Serial.println("meshCollection is still running");
      #endif
    }
  }else{
    #ifdef _debugopra
      if(runningMeshCollection){Serial.print("Running Mesh Collection!");}
    #endif
  }
}

struct AxisInfo{int X; int Y; int Z;};
AxisInfo axisInfo;



bool getAxisPostionInfo(){
  
  if(PollAxisInfo.check()){
    String response = api.getOctoprintEndpointResults("plugin/helloworld");
    StaticJsonDocument<512> root;
    
    bool error = deserializeJson(root, response);
    if (!error) {
      axisInfo.X = root["Axis"]["X"];
      axisInfo.Y = root["Axis"]["Y"];
      axisInfo.Z = root["Axis"]["Z"];

      #ifdef _debugopra
        Serial.print("axisInfo.X:"); Serial.println(axisInfo.X); Serial.print("axisInfo.Y:"); Serial.println(axisInfo.Y); Serial.print("axisInfo.Z:"); Serial.println(axisInfo.Z);
      #endif
      
      PollAxisInfo.set(PollAxisInfoInderval);
      return true;
    }else{
      #ifdef _debugopra
        Serial.println("Error getting Printer Axis position details.\n");
        if(error){ Serial.println("Deserialization error in getAxisPostionInfo");}
        
        Serial.println("Possible octoprint api error in getAxisPostionInfo");
        Serial.print("Status Code:");
        Serial.print("Status Code:");
        Serial.println(api.httpStatusCode);
        Serial.println("Response Body:");
        Serial.println(api.httpErrorBody);
      #endif 
    }
  }
  return false;
}

bool getPrinterConnectionInfo(){
  //call generic endpoint method...
  String response = api.getOctoprintEndpointResults("connection");
  StaticJsonDocument<1024> root;
  if (!deserializeJson(root, response)) {
    currentCon.state = (const char *)root["current"]["state"]; // "Opening serial connection","Operational","Closed"
    currentCon.profileId = (const char *)root["current"]["printerProfile"]; //"_default" is if no profile is selected. It maybe the only profile in the system. 
    currentCon.port = (const char *)root["current"]["port"];
    currentCon.baudrate = root["current"]["baudrate"];
    
    #ifdef _debugopra
    Serial.print("currentCon.state:");
    Serial.println(currentCon.state);

    Serial.print("currentCon.profileId:");
    Serial.println(currentCon.profileId);

    Serial.print("currentCon.port:");
    Serial.println(currentCon.port);
    #endif
    
    return true;
  }else{
    #ifdef _debugopra
    Serial.println("Error getting Printer Serial Conneciton details.");
    Serial.print("Status Code:");
    Serial.println(api.httpStatusCode);
    Serial.println("Response Body:");
    Serial.println(api.httpErrorBody);
    #endif 
  }
  return false;
}

bool OctoPrintOnline = true;
bool isOctoPrintOnline(){
  if(PollOPOnline.check()){
    OctoPrintOnline = api.getOctoprintVersion();
    PollOPOnline.set(PollOPOnlineInterval);
  }
  return OctoPrintOnline;
}
