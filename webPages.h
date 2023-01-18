String LastStatus = "Ready!"; //latest status message.
String Networks = "";




//takes the value from the wifi scan and turns it into a percentage signal strength
int dBmtoPercentage(int dBm)
{
  int quality;
    if(dBm <= -100)
    {
        quality = 0;
    }
    else if(dBm >= -50)
    {  
        quality = 100;
    }
    else
    {
        quality = 2 * (dBm + 100);
   }

     return quality;
}//dBmtoPercentage 



void storeWifiScanResult(int networksFound) {
  
  Networks = "{\"networks\":[";
  for (int i = 0; i < networksFound; i++){

    if((i + 1)< networksFound){
      Networks += "\"";
      Networks += WiFi.SSID(i);
      Networks += " (";
      Networks += dBmtoPercentage(WiFi.RSSI(i));
      Networks += "%)\",";
    }else{
      Networks += "\"";
      Networks += WiFi.SSID(i);
      Networks += " (";
      Networks += dBmtoPercentage(WiFi.RSSI(i));
      Networks += "%)\"";
    }
    #ifdef _debugopra
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    #endif
    
  }
  
  Networks += "]}";
  
  //WiFi.scanDelete(); not supported for esp8266
  
}

const char* www_realm = "Login Required";
String authFailResponse = "Authentication Failed";
bool checkAuth(){
  
  if(!webServer.authenticate(www_username,wificonfig.apPassword)){
    #ifdef USE_DIGESTAUTH
    webServer.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      #ifdef _debugopra
      Serial.print("AuthFailed?:");
      Serial.println(authFailResponse);
      #endif
    #else
    webServer.requestAuthentication();
    #endif

    return webServer.authenticate("admin",wificonfig.apPassword);
    
  }else{
    return true;  
  }
  
}

bool handleFileRead(String path) {
  if(!checkAuth())
    return false;
    
  #ifdef _debugopra
  Serial.println("start handleFileRead");
  #endif
  
  if (SPIFFS.exists(path)) {
    #ifdef _debugopra
    Serial.print("File found:");
    Serial.println(path);
    #endif
    File file = SPIFFS.open(path, "r");
    if(path.endsWith("css")){
      webServer.streamFile(file, "text/css");
    }else{
      webServer.streamFile(file, "text/html");
    }
    file.close();
    return true;
  }else{
    #ifdef _debugopra
    Serial.print("File not found");
    Serial.println(path);
    #endif
  }
  return false;
}

File fsUploadFile;  //SPIFFS file object to hold the new file info.
void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = webServer.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    LastStatus = filename; //put this here for use later when done the upload.
    if(!filename.startsWith("/")) filename = "/"+filename;
    #ifdef _debugopra
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    #endif
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      #ifdef _debugopra
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      #endif
      
      LastStatus = "File Upload success:" + LastStatus + ":" + String(upload.totalSize) + " bytes"; 
      webServer.sendHeader("Location","/spiffs");      // Redirect so that it refreshes
      webServer.send(303);
    } else {
      webServer.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

void GetFileList(String &PageContent){
  Dir dir = SPIFFS.openDir ("/");
  PageContent += "<p>";
  while (dir.next()) {
      PageContent += "<a href='/spiffsDL?download=";
      PageContent += dir.fileName();
      PageContent += "'>";
      PageContent += dir.fileName();
      PageContent += " ";
      PageContent += dir.fileSize();
      PageContent += "</a>";
      PageContent += "<br/>";
  }
  
  PageContent += "<p>";
  return;
}


 
 void getSpiffsPage(String &page){
    page = "<!DOCTYPE HTML><html><head><title>SPIFFS UPload</title><body>";
    page += "<form method='post' enctype='multipart/form-data'><input type='file' name='name'><input class='button' type='submit' value='Upload'></form>";
    GetFileList(page);
    page += "<p>Last Status: " + LastStatus + "</p>";
    page += "<p><a href='/config'>Return to Config</a></p>";
    page += "</body></html>";
}

bool readFileListToJsonArray(String& outPut){
    
    Dir dir = SPIFFS.openDir ("/");
    while (dir.next()) {
        outPut += "{\"path\":\"";
        outPut += dir.fileName();
        outPut += "\",\"size\":\"";
        outPut += dir.fileSize();
        outPut += "\"},";
    }
    outPut = outPut.substring(0,outPut.length()-1); //remove the last comma
    return true;
}

void initialisePages(){
  #ifdef _debugopra
  Serial.println("Initializing pages start");
  #endif
  
  webServer.serveStatic("/mansettings.js",SPIFFS,"/mansettings.js");//.setAuthentication(www_username,wificonfig.apPassword);
  webServer.serveStatic("/style.css",SPIFFS,"/style.css");//.setAuthentication(www_username,wificonfig.apPassword);//31536000
  webServer.serveStatic("/scripts.js",SPIFFS,"/scripts.js");//.setAuthentication(www_username,wificonfig.apPassword);

  webServer.on("/APIGetFileList",HTTP_GET,[]{
    if(!checkAuth()){return ;}

    String files = "[";
    readFileListToJsonArray(files);
    files += "]";
    webServer.send(200, "application/json", files);
  });



  webServer.on("/config",HTTP_GET,[]{
    #ifdef _debugopra
    Serial.println("start mansettings");
    #endif
    
    handleFileRead("/mansettings.htm");
    
    #ifdef _debugopra
    Serial.println("end mansettings");
    #endif
  });

  webServer.on("/config",HTTP_POST,[]{
    if(!checkAuth()){return;}

    if(webServer.hasArg("update")){
      #ifdef _debugopra
      Serial.println("updating settings");
      #endif
      
      strlcpy(wificonfig.ssid, webServer.arg("tx_SSID").c_str(), sizeof(wificonfig.ssid));
      strlcpy(wificonfig.password, webServer.arg("tx_WFPW").c_str(), sizeof(wificonfig.password));
      strlcpy(wificonfig.hostname, webServer.arg("tx_DNAM").c_str(), sizeof(wificonfig.hostname));
      strlcpy(wificonfig.apPassword, webServer.arg("tx_APW").c_str(), sizeof(wificonfig.apPassword));
      strlcpy(wificonfig.wifimode, webServer.arg("cbo_WFMD").c_str(), sizeof(wificonfig.wifimode));
      
      wificonfig.attempts = webServer.arg("tx_WFCA").toInt();
      wificonfig.attemptdelay = webServer.arg("tx_WFAD").toInt();
      
      #ifdef _debugopra
      Serial.println("AfterUpdate: wificonfig");
      Debugwificonfig();
      #endif
      

      //MQTT settings
      mqttConfig.MQTTEN = webServer.arg("ch_MQTTEN").toInt();
      mqttConfig.StatusIntervalSec = webServer.arg("tx_STI").toInt();
      strlcpy(mqttConfig.MQTTSrvIp, webServer.arg("tx_MQTTSRV").c_str(), sizeof(mqttConfig.MQTTSrvIp));
      strlcpy(mqttConfig.MQTTUser, webServer.arg("tx_MQTTUSR").c_str(), sizeof(mqttConfig.MQTTUser));
      strlcpy(mqttConfig.MQTTPW, webServer.arg("tx_MQTTPW").c_str(), sizeof(mqttConfig.MQTTPW));
      mqttConfig.MQTTPort = webServer.arg("tx_MQTTPort").toInt();
      
      strlcpy(settingsConfig.OPURI, webServer.arg("tx_OPURI").c_str(), sizeof(settingsConfig.OPURI));
      strlcpy(settingsConfig.OPAK, webServer.arg("tx_OPAK").c_str(), sizeof(settingsConfig.OPAK));
      strlcpy(settingsConfig.OPLGCF, webServer.arg("tx_OPLGCF").c_str(), sizeof(settingsConfig.OPLGCF));
      
      strlcpy(settingsConfig.unique_Id, webServer.arg("tx_HAUID").c_str(), sizeof(settingsConfig.unique_Id));
      strlcpy(settingsConfig.PProfile, webServer.arg("tx_PPFL").c_str(), sizeof(settingsConfig.PProfile));
      
      settingsConfig.BVX = webServer.arg("tx_BVX").toInt();
      settingsConfig.BVY = webServer.arg("tx_BVY").toInt();
      settingsConfig.BVO = webServer.arg("tx_BVO").toInt();

      settingsConfig.TimeZoneOffsetHours = webServer.arg("tx_TZOS").toInt();
      
      settingsConfig.OPPort = webServer.arg("tx_OPPort").toInt();

      if(webServer.hasArg("ch_OPReload")){
        settingsConfig.OPReload = 1;  
      }else{
        settingsConfig.OPReload = 0;  
      }
      
      if(webServer.hasArg("ch_OPStart")){
        settingsConfig.OPStart = 1;  
      }else{
        settingsConfig.OPStart = 0;  
      }
      
      if(webServer.hasArg("ch_BLAZ")){
        settingsConfig.BLAZ = 1;  
      }else{
        settingsConfig.BLAZ = 0;  
      }
      
      if(webServer.hasArg("ch_ACONP")){
        settingsConfig.ACONP = 1;  
      }else{
        settingsConfig.ACONP = 0;  
      }
      
      if(webServer.hasArg("ch_EML")){
        settingsConfig.EML = 1;  
      }else{
        settingsConfig.EML = 0;  
      }      
      
      
      #ifdef _debugopra
      Serial.println("AfterUpdate: SettingsConfig");
      DebugConfigSettings(); //prints the settings struct to the Serial line
      #endif 
      
      saveWifiConfig(wificonfig);
      saveSettingsConfig(settingsConfig);
      saveMQTTConfig(mqttConfig);
      
      LastStatus = "Update Completed(Ready!)";
      
    }else if(webServer.hasArg("reboot")){
      #ifdef _debugopra
      Serial.println("web called a reboot");
      #endif
      
      LastStatus = "Rebooting in 5 Sec";
      resetTimeDelay.set(10000UL); //need to give it more time so that the web page shows the message.
      needReset = true;
      
    }
    
    handleFileRead("/mansettings.htm");
    #ifdef _debugopra
    Serial.println("end mansettings");
    #endif
    
  });
  
  webServer.on("/APIDelFile",HTTP_GET,[]{
    if(!checkAuth()){return ;}

    LastStatus = "APIDelFile Called";
    if(webServer.hasArg("path")){
      String filePath = webServer.arg("path").c_str();
      #ifdef _debugopra
        Serial.println(filePath);
      #endif
      //protect some files
      //filePath == "/Control1.bmp" ||filePath == "/Home.bmp" ||filePath == "/Level.bmp" || filePath == "/Temps.bmp" ||
      if(filePath == "/mansettings.htm" ||filePath == "/mansettings.js" ||filePath == "/ota.htm" ||filePath == "/scripts.js" ||
        filePath == "/config/wificonfig.json" ||filePath == "/config/settings.json" ||filePath == "/style.css" ||filePath == "/spiffs.htm"){
        LastStatus = "Cannot deleted file: " + filePath;}
      else if(SPIFFS.exists(filePath)){
        if(SPIFFS.remove(filePath)){ //delete the file. 
          LastStatus = "File Deleted: " + filePath;
        }else{
          LastStatus = "Failed to Deleted file: " + filePath;
        }
      }
    }else{
      LastStatus = "Failed to Deleted file, no path provided";
    }
    
    webServer.send(200, "application/json", "{\"LastStatus\":\""+LastStatus+"\"}");
  });
    
  webServer.on("/APIGetSettings",HTTP_GET,[]{
    if(!checkAuth()){return ;}
      
    String settings = "{\"settings\":{";

    settings += "\"ssid\":\"" + String(wificonfig.ssid) + "\"";
    settings += ",\"password\":\"" + String(wificonfig.password) + "\"";
    settings += ",\"wifimode\":\"" + String(wificonfig.wifimode) + "\"";
    settings += ",\"hostname\":\"" + String(wificonfig.hostname) + "\"";
    settings += ",\"attempts\":\"" + String(wificonfig.attempts) + "\"";
    settings += ",\"attemptdelay\":\"" + String(wificonfig.attemptdelay) + "\"";
    settings += ",\"apPassword\":\"" + String(wificonfig.apPassword) + "\"";
    settings += ",\"LastStatus\":\"" + LastStatus + "\"";

    settings += ",\"MQTTEN\":\"" + String(mqttConfig.MQTTEN) + "\"";
    settings += ",\"StatusIntervalSec\":\"" + String(mqttConfig.StatusIntervalSec) + "\"";
    settings += ",\"unique_Id\":\"" + String(settingsConfig.unique_Id) + "\"";
    settings += ",\"MQTTPort\":\"" + String(mqttConfig.MQTTPort) + "\"";
    settings += ",\"MQTTEN\":\"" + String(mqttConfig.MQTTEN) + "\"";
    
    settings += ",\"MQTTSrvIp\":\"" + String(mqttConfig.MQTTSrvIp) + "\"";
    settings += ",\"MQTTUser\":\"" + String(mqttConfig.MQTTUser) + "\"";
    settings += ",\"MQTTPW\":\"" + String(mqttConfig.MQTTPW) + "\"";

    settings += ",\"OPURI\":\"" + String(settingsConfig.OPURI) + "\"";
    settings += ",\"OPPort\":\"" + String(settingsConfig.OPPort) + "\"";
    settings += ",\"OPAK\":\"" + String(settingsConfig.OPAK) + "\"";
    settings += ",\"OPLGCF\":\"" + String(settingsConfig.OPLGCF) + "\"";
    
    settings += ",\"OPReload\":\"" + String(settingsConfig.OPReload) + "\"";
    settings += ",\"OPStart\":\"" + String(settingsConfig.OPStart) + "\"";
    settings += ",\"BLAZ\":\"" + String(settingsConfig.BLAZ) + "\"";
    settings += ",\"EML\":\"" + String(settingsConfig.EML) + "\"";
    
    settings += ",\"ACONP\":\"" + String(settingsConfig.ACONP) + "\"";
    settings += ",\"PProfile\":\"" + String(settingsConfig.PProfile) + "\"";
    
    
    settings += ",\"BVX\":\"" + String(settingsConfig.BVX) + "\"";
    settings += ",\"BVY\":\"" + String(settingsConfig.BVY) + "\"";
    settings += ",\"BVO\":\"" + String(settingsConfig.BVO) + "\"";
    
    settings += ",\"TimeZoneOffsetHours\":\"" + String(settingsConfig.TimeZoneOffsetHours) + "\"";
    settings += ",\"firmwareV\":\"" + String(CONFIG_VERSION)+ "\"";
    settings += "}}";
    
    webServer.send(200, "application/json", settings);
  });
  
  webServer.on("/getDeviceName",HTTP_GET,[]{
  if(!checkAuth()){ return;}
    
    String msg = String(wificonfig.hostname) + ": " + String(CONFIG_VERSION);
    
    webServer.send(200, "text/plain", msg);
  });

  webServer.on("/APIScanWifi",HTTP_GET,[]{
    if(!checkAuth()){return ;}
      
    //int n = WiFi.scanNetworks();
    //storeWifiScanResult(n);
    doWiFiScan = true;
    #ifdef _debugopra
    Serial.print("Network Scanned requested:");
    #endif
    webServer.send(200, "text/plain","OK" );
  });

  webServer.on("/APIGetNetworks",HTTP_GET,[]{
  if(!checkAuth()){return;}
    if(Networks.length() == 0){
      #ifdef _debugopra
      Serial.println("Sending Empty Network list");
      #endif
      webServer.send(200, "application/json", "{\"networks\":[]}");
    }else{
      #ifdef _debugopra
      Serial.println("Sending Network list");
      #endif
      
      webServer.send(200, "application/json", Networks);
      Networks = "";  
    }
  });

  webServer.on("/spiffs",HTTP_GET,[]{
    if(!checkAuth()){return;}
    String page = "";
    if(!SPIFFS.exists("/spiffs.htm")){
      getSpiffsPage(page);
      webServer.send(200, "text/html",page );
    }else{
     handleFileRead("/spiffs.htm"); 
    }
  });

  webServer.on("/spiffs", HTTP_POST,[](){ 
    if(!checkAuth()){return;}
    
    webServer.send(200);},// Send status 200 (OK) to tell the client we are ready to receive
    handleFileUpload      // Receive and save the file
  );

  webServer.on("/spiffsDL",HTTP_GET,[]{
    if(!checkAuth()){return;}
    
    if(webServer.hasArg("download")){
       String fileName = webServer.arg("download").c_str();
       handleFileRead(fileName);
    }
  });
  #ifdef _debugopra
  Serial.println("Initializing pages completed");
  #endif 
}

     
