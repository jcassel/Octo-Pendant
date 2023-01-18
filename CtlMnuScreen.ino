#define CtlMnuARgnSize  6
ActionRegion CtlMnuARgn[CtlMnuARgnSize];
#define CTLMR 0
#define CS 1
#define TS 2
#define LVL 3
#define FC 4
#define XX 5
//ctlmnuscrn.json

bool CtlMnuRgnPushed(int x, int y){
  for (uint8_t rgn = 0; rgn < CtlMnuARgnSize; rgn++) {   
    if(CtlMnuARgn[rgn].contains(x,y,true)){//true here causes the hooked event to fire if this return true. 
      #ifdef _debugopra
      Serial.println(rgn); //finished test event text assertion
      #endif
      return true;
    }
  }
  return false;
}

void CtlMnuARgn_CTLMR(){ //0 CTLMR
  #ifdef _debugopra
  Serial.println("CtlMnuARgn_R");
  #endif
  currentScreen = 0;
  initHomeScreen();
  
}

void CtlMnuARgn_CS(){ //1
  #ifdef _debugopra
  Serial.println("CtlMnuARgn_CS");
  #endif
  currentScreen = 1;
  initControlScreen();
}

/*
 * Nav to Temperature control screen
 */
void CtlMnuARgn_TS(){ //2
  #ifdef _debugopra
  Serial.println("CtlMnuARgn_TS");
  #endif
  currentScreen = 2;
  initTempScreen();
}

void CtlMnuARgn_LVL(){ //3
  #ifdef _debugopra
  Serial.println("CtlMnuARgn_LVL");
  #endif
  currentScreen = 3;
  initLevelScreen();
  if(settingsConfig.BLAZ == 1){
    #ifdef _debugopra
      Serial.println("Invoking Auto Home all(XYZ) on screen entry, Using \"lvlscrnRgns_4()\"");
    #endif
    lvlscrnRgns_HXYZ(); //home all Axis
  }
}

void CtlMnuARgn_FC(){ //4
  #ifdef _debugopra
  Serial.println("CtlMnuARgn_FC");
  #endif
  currentScreen = 6;
  initfilCtl();
}

void CtlMnuARgn_XX(){ //5
  #ifdef _debugopra
  Serial.println("CtlMnuARgn_XX");
  #endif
  
  
}

bool loadCtlMScreenRgns(){
  
  const char* sourceFile = "/ctlmnuscrn.json";
  bool fileExists = SPIFFS.exists(sourceFile);
  
  char tmpSRgn[13];   
  strlcpy(tmpSRgn, "025015090090", 13); //<< plug in the same value you would have from your file if you want to change this at this level.
  Region tmpRgn = getRegionFromString(tmpSRgn);
  CtlMnuARgn[CTLMR].TextRgn = false;
  CtlMnuARgn[CTLMR].init(tmpRgn);
    
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
  //Return back (over rides the above when the file exists) 
  strlcpy(tmpSRgn, doc["CTLMR"] | "025015090090", 13);
  tmpRgn = getRegionFromString(tmpSRgn);
  CtlMnuARgn[CTLMR].TextRgn = false;
  CtlMnuARgn[CTLMR].init(tmpRgn);
  
    //CS
    strlcpy(tmpSRgn, doc["CS"] | "130015090090", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    CtlMnuARgn[CS].TextRgn = false;
    CtlMnuARgn[CS].init(tmpRgn);    

    //TS
    strlcpy(tmpSRgn, doc["TS"] | "025115090090", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    CtlMnuARgn[TS].TextRgn = false;
    CtlMnuARgn[TS].init(tmpRgn);    

    //LVL
    strlcpy(tmpSRgn, doc["LVL"] | "130115090090", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    CtlMnuARgn[LVL].TextRgn = false;
    CtlMnuARgn[LVL].init(tmpRgn);    

    //FC
    strlcpy(tmpSRgn, doc["FC"] | "025215090090", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    CtlMnuARgn[FC].TextRgn = false;
    CtlMnuARgn[FC].init(tmpRgn);                

    //XX
    strlcpy(tmpSRgn, doc["XX"] | "130215090090", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    CtlMnuARgn[XX].TextRgn = false;
    CtlMnuARgn[XX].init(tmpRgn);                    
    
    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < CtlMnuARgnSize; rgn++) {   
      Serial.println(CtlMnuARgn[rgn].getRegionDebug());
      CtlMnuARgn[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  
  
  return true;

}


void setCtlMnuScreenText(){
  //Currently No on screen text to update. 
  #ifdef _debugopra
  Serial.println("setCtlMnuScreenText");
  #endif
}

void initCtlMnu(){
  #ifdef _debugopra
  Serial.println("Running initCtlMnu, Screen 5");
  #endif

  tft.fillScreen(TFT_BLACK);            // Clear screen
  drawBmp("/ControlMenu.bmp", 0, 0); //
  loadCtlMScreenRgns();
  
  setCtlMnuScreenText();
  
  //setup buttons
  for (uint8_t rgn = 0; rgn < CtlMnuARgnSize; rgn++) { 
    //CtlMnuARgn[rgn].init(CtlMnuBtnRgn[rgn]);
    #ifdef _debugopra
    CtlMnuARgn[rgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
    #endif
    if(rgn == CTLMR){CtlMnuARgn[rgn].setReleaseAction(CtlMnuARgn_CTLMR);}
    else if(rgn==CS){CtlMnuARgn[rgn].setReleaseAction(CtlMnuARgn_CS);}
    else if(rgn==TS){CtlMnuARgn[rgn].setReleaseAction(CtlMnuARgn_TS);}
    else if(rgn==LVL){CtlMnuARgn[rgn].setReleaseAction(CtlMnuARgn_LVL);}
    else if(rgn==FC){CtlMnuARgn[rgn].setReleaseAction(CtlMnuARgn_FC);}
    else if(rgn==XX){CtlMnuARgn[rgn].setReleaseAction(CtlMnuARgn_XX);}
    
  }
}
