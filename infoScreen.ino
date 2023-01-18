/*info screen*/

#define infoScrnRgnSize  4
ActionRegion infoScrnARgn[infoScrnRgnSize];

//#define returnRgn 0 already defined.
#define nameRgn   1
#define ipRgn     2
#define resetRgn  3


TimeRelease resetEnableDelay;
bool infoRgnPushed(int x, int y){
  
  if(resetEnableDelay.check()|| !resetEnableDelay.isRunning()){
    for (uint8_t rgn = 0; rgn < infoScrnRgnSize; rgn++) {   
      if(infoScrnARgn[rgn].contains(x,y,true)){ //true here causes the hooked event to fire if this return true. 
        #ifdef _debugopra
        Serial.print("infoRgnPushed: ");
        Serial.println(rgn); //finished test event text assertion
        #endif
        return true; //could remove this so that if you have over laping items, it can call all of them as pressed. Need to make sure to return true somehow though. 
      }
    }
  }else{
      #ifdef _debugopra
      //Serial.println("Delaying");
      #endif
  }
  
  return false;
}


void infoScrnARgn_hm(){//send user to home screen.
  #ifdef _debugopra
  Serial.println("infoScrnARgn_hm");
  #endif
  currentScreen = 0;
  initHomeScreen();
  
}

void infoScrnARgn_Rst(){
  //set reset trigger.
  #ifdef _debugopra
  Serial.println("infoScrnARgn_Rst");
  #endif

  resetTimeDelay.set(500UL); //need to give it more time so that the web page shows the message.
  needReset = true;
}

void setInfoScreenText(){
  //update text on info screen.
  Serial.println("setInfoScreenText()");
  //setup text regions
  tft.setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
  tft.setTextColor(TFT_YELLOW);     // Set the font colour
  String displayString = "";
  String leadDisplayString = "";
  
  
  tft.setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
  tft.setTextColor(TFT_YELLOW);     // Set the font colour

  //infoScrnARgn[0] has no text!
  
  
  leadDisplayString = "HostName: "; displayString = String(wificonfig.hostname);
  tft.fillRect(infoScrnARgn[nameRgn].region.x, infoScrnARgn[nameRgn].region.y, infoScrnARgn[nameRgn].region.w, infoScrnARgn[nameRgn].region.h, TFT_BLACK);
  tft.setTextColor(TFT_WHITE);     // Set the font colour
  infoScrnARgn[nameRgn].region.w = tft.drawString(leadDisplayString,infoScrnARgn[nameRgn].region.x,infoScrnARgn[nameRgn].region.y); //return is the new length of the displayed text.
  tft.setTextColor(TFT_YELLOW);     // Set the font colour
  infoScrnARgn[nameRgn].region.w += tft.drawString(displayString,infoScrnARgn[nameRgn].region.x + infoScrnARgn[nameRgn].region.w,infoScrnARgn[nameRgn].region.y); //return is the new length of the displayed text.

  leadDisplayString = "IP: ";displayString = IpAddress2String(WiFi.localIP());
  tft.setTextColor(TFT_WHITE);     // Set the font colour
  infoScrnARgn[ipRgn].region.w = tft.drawString(leadDisplayString,infoScrnARgn[ipRgn].region.x,infoScrnARgn[ipRgn].region.y); //return is the new length of the displayed text.
  tft.setTextColor(TFT_YELLOW);     // Set the font colour
  infoScrnARgn[ipRgn].region.w += tft.drawString(displayString,infoScrnARgn[ipRgn].region.x + infoScrnARgn[ipRgn].region.w,infoScrnARgn[ipRgn].region.y); //return is the new length of the displayed text.
  
  leadDisplayString = "Reset Pendant";displayString = "";
  //tft.setTextColor(TFT_WHITE);     // Set the font colour
  infoScrnARgn[resetRgn].displayText(&tft,leadDisplayString,TFT_BLACK,TFT_WHITE);

    #ifdef _debugopra
    infoScrnARgn[nameRgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
    infoScrnARgn[ipRgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
    infoScrnARgn[resetRgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
    #endif


  
}



bool loadInfoScreenRgns(){
  
  const char* sourceFile = "/infoscrn.json";
  
  bool fileExists = SPIFFS.exists(sourceFile);
  
  char tmpSRgn[13];   
  strlcpy(tmpSRgn, "001001240280", 13); //<< plug in the same value you would have from your file if you want to change this at this level.
  Region tmpRgn = getRegionFromString(tmpSRgn);
  infoScrnARgn[returnRgn].TextRgn = false;
  infoScrnARgn[returnRgn].init(tmpRgn);
    
  if(!fileExists){
    #ifdef _debugopra
      Serial.println("[WARNING]: Config file:");Serial.print(sourceFile);Serial.println(" not found");
    #endif
  }
  #ifdef _debugopra
  Serial.print("Loading:");Serial.println(sourceFile);
  #endif
  File configfile = SPIFFS.open(sourceFile,"r");
  DynamicJsonDocument doc(1024);  
  DeserializationError error = deserializeJson(doc, configfile);
  
  if (error){Serial.println("[ERROR]: deserializeJson() error in lvlscrn.json");Serial.println(error.c_str());}

  #ifdef _debugopra
    Serial.println("Rendering Screen Regions");
  #endif

    
    strlcpy(tmpSRgn, doc["R"] | "001001240280", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    infoScrnARgn[returnRgn].TextRgn = false;
    infoScrnARgn[returnRgn].init(tmpRgn);
    
    //Name 
    strlcpy(tmpSRgn, doc["N"] | "005090150020", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    infoScrnARgn[nameRgn].TextRgn = false;
    infoScrnARgn[nameRgn].init(tmpRgn);    

    //Ip Address
    strlcpy(tmpSRgn, doc["IP"] | "005115150020", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    infoScrnARgn[ipRgn].TextRgn = false;
    infoScrnARgn[ipRgn].init(tmpRgn);    

    //Reset Restart
    strlcpy(tmpSRgn, doc["RS"] | "001290240040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    infoScrnARgn[resetRgn].TextRgn = false;
    infoScrnARgn[resetRgn].init(tmpRgn);    
    
        
    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < infoScrnRgnSize; rgn++) {   
      Serial.println(infoScrnARgn[rgn].getRegionDebug());
      infoScrnARgn[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  
  
  return true;
}



void initInfoScreen(){
  
  #ifdef _debugopra
  Serial.println("Running initInfoScreen, Screen 4");
  #endif
  Serial.println("SetResetdelay");
  resetEnableDelay.set(500); //delay to stop a user from clicking the reset button by mistake when trying to navigate to the info screen.
  
  tft.fillScreen(TFT_BLACK);            // Clear screen
  //drawBmp("/info.bmp", 0, 0); //no bmp for this screen.

  loadInfoScreenRgns();
  setInfoScreenText();
  
  //setup buttons
  for (uint8_t rgn = 0; rgn < infoScrnRgnSize; rgn++) { 
    //infoScrnARgn[rgn].init(infoBtnRgs[rgn]);

    if(rgn == 0){//0 return home
      infoScrnARgn[rgn].setReleaseAction(infoScrnARgn_hm);
    }else if(rgn == 1){//1 this will not get hit anyway since region 0 will cover all text and it will be the first region checked.
      //Name 
    }else if(rgn == 2){//2 this will not get hit anyway since region 0 will cover all text and it will be the first region checked.
      //IP Address
    }else if(rgn == 3){//3 reboot/reset
      infoScrnARgn[rgn].setReleaseAction(infoScrnARgn_Rst);
    }
    #ifdef _debugopra
    infoScrnARgn[rgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
    #endif
  }
  
  
}


/****End info screen***/
