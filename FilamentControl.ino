#define filCtlRgnSize  15
ActionRegion filCtlARgn[filCtlRgnSize];

#define returnRgn 0
#define automateRgn 1
#define eHeatOffRgn 2
#define heat1Rgn 3
#define heat2Rgn 4
#define heat3Rgn 5
#define heat4Rgn 6
#define UnldbtnRgn 7
#define Unl1Rgn 8
#define Unl2Rgn 9
#define Unl3Rgn 10
#define Unl4Rgn 11
#define LdbtnRgn 12
#define curEvalsRgn 13
#define curEDisRgn 14

int unloadLen = 400; //should be a setting. 

bool rgnsLoaded = false;
bool filCtlRgnPushed(int x, int y){
  if(rgnsLoaded){
    for (uint8_t rgn = 0; rgn < filCtlRgnSize; rgn++) {   
      if(filCtlARgn[rgn].contains(x,y,true)){//true here causes the hooked event to fire if this return true. 
        #ifdef _debugopra
        Serial.println(rgn); //finished test event text assertion
        #endif
        return true;
      }
    }
  }
  return false;
}

void filCtlARgn_R(){ //0
  #ifdef _debugopra
  Serial.println("filCtlRgnAction_hm");
  #endif
  currentScreen = 5;
  initCtlMnu();
}

void filCtlARgn_A(){ //1
  #ifdef _debugopra
  Serial.println("filCtlARgn_1");
  #endif
  char commandData[100];
  String commandString = "M600 B10 U400 L100 X"+String(settingsConfig.BVX-settingsConfig.BVO) +" Y"+String(settingsConfig.BVO)+ " Z40";
  Serial.print("FilamentOut Command:");
  Serial.println(commandString);
  //"M600 B10 U400 L100 X[front] Y[right] Z40[Relative]
  strlcpy(commandData, commandString.c_str(), sizeof(commandData));
  api.octoPrintPrinterCommand(commandData);
  
}


void filCtlARgn_E(){ //Extruder heat off
  #ifdef _debugopra
  Serial.println("filCtlARgn_E");
  #endif
  api.octoPrintSetTool0Temperature(0);
}

void filCtlARgn_H1(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_H1");
  #endif
  api.octoPrintSetTool0Temperature(170);
}

void filCtlARgn_H2(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_H2");
  #endif
  api.octoPrintSetTool0Temperature(180);
}


void filCtlARgn_H3(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_H3");
  #endif
  api.octoPrintSetTool0Temperature(200);
}

void filCtlARgn_H4(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_H4");
  #endif
  api.octoPrintSetTool0Temperature(230);
  
}

void filCtlARgn_Ulb(){ //unload button Up Arrow
  #ifdef _debugopra
  Serial.println("filCtlARgn_Ulb");
  #endif
  setfilCtlScreenText();
}

void filCtlARgn_Ul1(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_Ul1");
  #endif
  unloadLen = 10;
  setfilCtlScreenText();
}

void filCtlARgn_Ul2(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_Ul2");
  #endif
  unloadLen = 100;
  setfilCtlScreenText();
}

void filCtlARgn_Ul3(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_Ul3");
  #endif
  unloadLen = 50;
  setfilCtlScreenText();
}

void filCtlARgn_Ul4(){ 
  #ifdef _debugopra
  Serial.println("filCtlARgn_Ul4");
  #endif
  unloadLen = 200;
  setfilCtlScreenText();
}

void filCtlARgn_Lb(){ //load button Down arrow
  #ifdef _debugopra
  Serial.println("filCtlARgn_Lb");
  #endif
  
}


bool loadFilamentScreenRgns(){
  const char* sourceFile = "/fSrgn.json";
  bool fileExists = SPIFFS.exists(sourceFile);
  
  char tmpSRgn[13];   
  strlcpy(tmpSRgn, "001001065050", 13); //<< plug in the same value you would have from your file if you want to change this at this level.
  Region tmpRgn = getRegionFromString(tmpSRgn);
  filCtlARgn[returnRgn].TextRgn = false;
  filCtlARgn[returnRgn].init(tmpRgn);
    
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
  
 
    //Return
    strlcpy(tmpSRgn, doc["R"] | "001001065050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[returnRgn].init(tmpRgn);
    filCtlARgn[returnRgn].setReleaseAction(filCtlARgn_R);
    
    //Automate 
    strlcpy(tmpSRgn, doc["A"] | "180001065050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[automateRgn].init(tmpRgn);
    filCtlARgn[automateRgn].setReleaseAction(filCtlARgn_A);
    
    //eHeatOff
    strlcpy(tmpSRgn, doc["E"] | "190070050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[eHeatOffRgn].init(tmpRgn);
    filCtlARgn[eHeatOffRgn].setReleaseAction(filCtlARgn_E);
    
    //Heat1
    strlcpy(tmpSRgn, doc["H1"] | "010115050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[heat1Rgn].init(tmpRgn);
    filCtlARgn[heat1Rgn].setReleaseAction(filCtlARgn_H1);
  
    //Heat2
    strlcpy(tmpSRgn, doc["H2"] | "070115050035", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[heat2Rgn].init(tmpRgn);
    filCtlARgn[heat2Rgn].setReleaseAction(filCtlARgn_H2);
  
    //Heat3
    strlcpy(tmpSRgn, doc["H3"] | "125115050035", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[heat3Rgn].init(tmpRgn);
    filCtlARgn[heat3Rgn].setReleaseAction(filCtlARgn_H3);
  
    //Heat4
    strlcpy(tmpSRgn, doc["H4"] | "190115050035", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[heat4Rgn].init(tmpRgn);   
    filCtlARgn[heat4Rgn].setReleaseAction(filCtlARgn_H4);

    //unloadbtn (up arrow)
    strlcpy(tmpSRgn, doc["Ulb"] | "160170060050", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[UnldbtnRgn].init(tmpRgn);   
    filCtlARgn[UnldbtnRgn].setReleaseAction(filCtlARgn_Ulb);

    //load heat 1
    strlcpy(tmpSRgn, doc["Ul1"] | "036198050035", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[Unl1Rgn].init(tmpRgn);       
    filCtlARgn[Unl1Rgn].setReleaseAction(filCtlARgn_Ul1);

    //load heat 2
    strlcpy(tmpSRgn, doc["Ul2"] | "098198050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[Unl2Rgn].init(tmpRgn);   
    filCtlARgn[Unl2Rgn].setReleaseAction(filCtlARgn_Ul2);
    
    //load heat 3
    strlcpy(tmpSRgn, doc["Ul3"] | "036238050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[Unl3Rgn].init(tmpRgn);   
    filCtlARgn[Unl3Rgn].setReleaseAction(filCtlARgn_Ul3);
    
    //load heat 4
    strlcpy(tmpSRgn, doc["Ul4"] | "098238050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    filCtlARgn[Unl4Rgn].init(tmpRgn);   
    filCtlARgn[Unl4Rgn].setReleaseAction(filCtlARgn_Ul4);
    
    //loadbtn
    strlcpy(tmpSRgn, doc["Lb"] | "160265060050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);    
    filCtlARgn[LdbtnRgn].init(tmpRgn); 
    filCtlARgn[LdbtnRgn].setReleaseAction(filCtlARgn_Lb);
    
    //CurEvals
    strlcpy(tmpSRgn, doc["CE"] | "091084065015", 13);
    tmpRgn = getRegionFromString(tmpSRgn);    
    filCtlARgn[curEvalsRgn].TextRgn = true;
    filCtlARgn[curEvalsRgn].init(tmpRgn); 
    //filCtlARgn[curEvalsRgn].setReleaseAction(filCtlARgn_CE);
    
    //CurEDis
    strlcpy(tmpSRgn, doc["CED"] | "172235035015", 13);
    tmpRgn = getRegionFromString(tmpSRgn);    
    filCtlARgn[curEDisRgn].TextRgn = true;
    filCtlARgn[curEDisRgn].init(tmpRgn); 
    //filCtlARgn[curEDisRgn].setReleaseAction(filCtlARgn_CED);
    

    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < filCtlRgnSize; rgn++) {   
      Serial.println(filCtlARgn[rgn].getRegionDebug());
      filCtlARgn[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  
  
  return true;

}

void setfilCtlScreenText(){
  apigetPrintStats();

  //String tmp = String(api.printerLastState.printerTool0TempActual);//better naming would be nice
  String tmp = String(api.printerStats.printerTool0TempActual);
  
  String tempDisplayTool0 = tmp.substring(0,tmp.indexOf("."));
  tempDisplayTool0 += "/";
  //tmp = String(api.printerLastState.printerTool0TempTarget);//better naming would be nice
  tmp = String(api.printerStats.printerTool0TempTarget);
  
  tempDisplayTool0 += tmp.substring(0,tmp.indexOf("."));
  
  filCtlARgn[curEvalsRgn].displayText(&tft,tempDisplayTool0,TFT_BLACK,TFT_YELLOW);
  filCtlARgn[curEDisRgn].displayText(&tft,String(unloadLen),TFT_BLACK,TFT_YELLOW);
}


void initfilCtl(){
  #ifdef _debugopra
  Serial.println("Running initfilCtl, Screen 6");
  #endif

  tft.fillScreen(TFT_BLACK);            // Clear screen
  drawBmp("/FilamentControl.bmp", 0, 0); //no bmp for this screen.
  //rgnsLoaded =loadScreenRegions(); 
  rgnsLoaded = loadFilamentScreenRgns();
  if(rgnsLoaded){
    setfilCtlScreenText();
 
    //setup buttons
    for (uint8_t rgn = 0; rgn < filCtlRgnSize; rgn++) { 
      //filCtlARgn[rgn].init(filCtlBtnRgn[rgn]); //now done in loadScreenRegions
      #ifdef _debugopra
      filCtlARgn[rgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
      #endif
    }
  }
}
