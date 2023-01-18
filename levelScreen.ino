#define lvlscrnRgnsSize 14
ActionRegion lvlscrnRgns[lvlscrnRgnsSize];


//#define returnRgn Already exists as Zero (0)
#define BOff 1
#define B50 2
#define B60 3
#define B100 4
#define HXYZ 5
#define BLF 6
#define BRF 7
#define BC 8
#define BLB 9
#define BRB 10
#define BCV 11
#define M0 12
#define M1 13


bool lvlRgnPushed(int x, int y){
  
  for (uint8_t rgn = 0; rgn < lvlscrnRgnsSize; rgn++) {   
    if(lvlscrnRgns[rgn].contains(x,y,true)){
      #ifdef _debugopra
      Serial.println(rgn); //finished test event text assertion
      #endif
      return true;
    }
  }
  return false;
}


void lvlscrnRgns_ReturnHome(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_0");
  #endif
  currentScreen = 5;
  initCtlMnu();
}

void lvlscrnRgns_1(){//Turn on he build plate bed. 
  apigetPrintStats();
  int tmp = String(api.printerStats.printerBedTempTarget).toInt();
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_1");
  Serial.print("Target bed temp: ");
  Serial.println(tmp);
  #endif
  if(tmp == 0){
    api.octoPrintSetBedTemperature(60); //this should come from a setting.
  }
  
  Serial.println("Alert... remove hardcoded bed temp (60); Screen.ino/lblscrnRgn_1");
  
}

void lvlscrnRgns_BOff(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_BOff");
  #endif
  api.octoPrintSetBedTemperature(0);
}


void lvlscrnRgns_B50(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_B50");
  #endif
  api.octoPrintSetBedTemperature(50);
}
void lvlscrnRgns_B60(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_B60");
  #endif
  api.octoPrintSetBedTemperature(60);
}
void lvlscrnRgns_B100(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_B100");
  #endif
  api.octoPrintSetBedTemperature(100);
}

void lvlscrnRgns_M0(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_M0");
  #endif
  if(settingsConfig.EML == 1){
    RunMeshCollection(false);
  }
}

void lvlscrnRgns_M1(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_M1");
  #endif
  if(settingsConfig.EML == 1){
    RunMeshCollection(true);
  }
}


void lvlscrnRgns_HXYZ(){
  #ifdef _debugopra
  Serial.println("lvlscrnRgns_HXYZ");
  #endif
  api.sendPostToOctoPrint("/api/printer/printhead","{\"command\": \"home\",\"axes\": [\"x\",\"y\",\"z\"]}");
}


void lvlRgnAction_BLF(){ //left front point
  //send xy to point (maybe lift z and drop it as well)
  #ifdef _debugopra
  Serial.println("lvlRgnAction_BLF");
  #endif
  if(!motionDisabled){
    api.octoPrintPrinterCommand("G1 Z5");
    api.octoPrintPrintHeadAbsoluteJog(settingsConfig.BVO,settingsConfig.BVO,5,defaultMotionSpeed);//x,y,z,f(speed) 
    api.octoPrintPrinterCommand("G1 Z0");
  }
}

void lvlRgnAction_BLB(){ //left back point
  //send xy to point (maybe lift z and drop it as well)
  #ifdef _debugopra
  Serial.println("lvlRgnAction_BLB");
  #endif
  if(!motionDisabled){
    api.octoPrintPrinterCommand("G1 Z5");
    api.octoPrintPrintHeadAbsoluteJog(settingsConfig.BVO,settingsConfig.BVY - settingsConfig.BVO,5,defaultMotionSpeed);//x,y,z,f(speed)  
    api.octoPrintPrinterCommand("G1 Z0");
  }
}

void lvlRgnAction_BRF(){ //right front point
  //send xy to point (maybe lift z and drop it as well)
  #ifdef _debugopra
  Serial.println("lvlRgnAction_BRF");
  #endif
  if(!motionDisabled){
    api.octoPrintPrinterCommand("G1 Z5");
    api.octoPrintPrintHeadAbsoluteJog(settingsConfig.BVX - settingsConfig.BVO,settingsConfig.BVO,5,defaultMotionSpeed);//x,y,z,f(speed)  
    api.octoPrintPrinterCommand("G1 Z0");
  }
}

void lvlRgnAction_BRB(){ //right back point
  //send xy to point (maybe lift z and drop it as well)
  #ifdef _debugopra
  Serial.println("lvlRgnAction_BRB");
  #endif
  if(!motionDisabled){
    api.octoPrintPrinterCommand("G1 Z5");
    api.octoPrintPrintHeadAbsoluteJog(settingsConfig.BVX - settingsConfig.BVO,settingsConfig.BVY - settingsConfig.BVO,5,defaultMotionSpeed);//x,y,z,f(speed)  
    api.octoPrintPrinterCommand("G1 Z0");
  }
}

void lvlRgnAction_BC(){ //center point
  //send xy to point (maybe lift z and drop it as well)
  #ifdef _debugopra
  Serial.println("lvlRgnAction_BC");
  #endif  
  if(!motionDisabled){
    api.octoPrintPrinterCommand("G1 Z5");
    api.octoPrintPrintHeadAbsoluteJog(settingsConfig.BVX/2,settingsConfig.BVY/2,5,defaultMotionSpeed);//x,y,z,f(speed)  
    api.octoPrintPrinterCommand("G1 Z0");
  }
}

bool loadLvlScreenRgns(){
  const char* sourceFile = "/lvlscrn.json";
  
  bool fileExists = SPIFFS.exists(sourceFile);
  
  char tmpSRgn[13];   
  strlcpy(tmpSRgn, "001001055050", 13); //<< plug in the same value you would have from your file if you want to change this at this level.
  Region tmpRgn = getRegionFromString(tmpSRgn);
  lvlscrnRgns[returnRgn].TextRgn = false;
  lvlscrnRgns[returnRgn].init(tmpRgn);
    
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
  strlcpy(tmpSRgn, doc["R"] | "001001055050", 13);
  tmpRgn = getRegionFromString(tmpSRgn);
  lvlscrnRgns[returnRgn].TextRgn = false;
  lvlscrnRgns[returnRgn].init(tmpRgn);
  
    //BOff
    strlcpy(tmpSRgn, doc["BOff"] | "100160055040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BOff].TextRgn = false;
    lvlscrnRgns[BOff].init(tmpRgn);    

    //B50
    strlcpy(tmpSRgn, doc["B50"] | "175160060040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[B50].TextRgn = false;
    lvlscrnRgns[B50].init(tmpRgn);

    //B60
    strlcpy(tmpSRgn, doc["B60"] | "010092058044", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[B60].TextRgn = false;
    lvlscrnRgns[B60].init(tmpRgn);    


    //B100
    strlcpy(tmpSRgn, doc["B100"] | "010140058040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[B100].TextRgn = false;
    lvlscrnRgns[B100].init(tmpRgn);

    //HXYZ
    strlcpy(tmpSRgn, doc["HXYZ"] | "191006040040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[HXYZ].TextRgn = false;
    lvlscrnRgns[HXYZ].init(tmpRgn);


    //BLF
    strlcpy(tmpSRgn, doc["BLF"] | "010183058044", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BLF].TextRgn = false;
    lvlscrnRgns[BLF].init(tmpRgn);

    //BRF
    strlcpy(tmpSRgn, doc["BRF"] | "012235050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BRF].TextRgn = false;
    lvlscrnRgns[BRF].init(tmpRgn);

    //BC
    strlcpy(tmpSRgn, doc["BC"] | "067235050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BC].TextRgn = false;
    lvlscrnRgns[BC].init(tmpRgn);

    //BLB
    strlcpy(tmpSRgn, doc["BLB"] | "124235050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BLB].TextRgn = false;
    lvlscrnRgns[BLB].init(tmpRgn);

    //BRB
    strlcpy(tmpSRgn, doc["BRB"] | "180235050035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BRB].TextRgn = false;
    lvlscrnRgns[BRB].init(tmpRgn);

    //BCV
    strlcpy(tmpSRgn, doc["BCV"] | "085006040040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[BCV].TextRgn = true;
    lvlscrnRgns[BCV].init(tmpRgn);

    //M0
    strlcpy(tmpSRgn, doc["M0"] | "005270055050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[M0].TextRgn = false;
    lvlscrnRgns[M0].init(tmpRgn);

    //M1
    strlcpy(tmpSRgn, doc["M1"] | "183270055050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    lvlscrnRgns[M1].TextRgn = false;
    lvlscrnRgns[M1].init(tmpRgn);
    
        
    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < lvlscrnRgnsSize; rgn++) {   
      Serial.println(lvlscrnRgns[rgn].getRegionDebug());
      lvlscrnRgns[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  
  
  return true;
}


void initLevelScreen(){
  #ifdef _debugopra
  Serial.println("Running initLevelScreen, Screen 3");
  #endif
  
  tft.fillScreen(TFT_BLACK);            // Clear screen
  drawBmp("/Level.bmp", 0, 0); //no bmp for this screen.
  if(!settingsConfig.EML){
    tft.fillRect(1,270,240,50,TFT_BLACK);//cover the Mesh Collection seciton.
  }
  loadLvlScreenRgns();
  
  
  //create regions for actions
  for (uint8_t rgn = 0; rgn < lvlscrnRgnsSize; rgn++) { 
    //lvlscrnRgns[rgn].init(lvlBtnRgns[rgn].x,lvlBtnRgns[rgn].y,lvlBtnRgns[rgn].w,lvlBtnRgns[rgn].h);
    #ifdef _debugopra
      lvlscrnRgns[rgn].drawRegion(&tft); // debug visually the regions.
    #endif
    if(rgn == returnRgn){
      lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_ReturnHome);
    }if(rgn == BOff){
      lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_BOff); 
    }if(rgn == B50){
      lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_B50);
    }if(rgn == B60){
      lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_B60);
    }if(rgn == B100){
      lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_B100);            
    }if(rgn == HXYZ){
      lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_HXYZ); //All Axis home 
    }if(rgn == BLF){// left front point
      lvlscrnRgns[rgn].setReleaseAction(lvlRgnAction_BLF); 
    }if(rgn == BRF){// right front point
      lvlscrnRgns[rgn].setReleaseAction(lvlRgnAction_BRF); 
    }if(rgn == BLB){// left back point
      lvlscrnRgns[rgn].setReleaseAction(lvlRgnAction_BLB); 
    }if(rgn == BRB){// right back point
      lvlscrnRgns[rgn].setReleaseAction(lvlRgnAction_BRB); 
    }if(rgn == BC){// center point
      lvlscrnRgns[rgn].setReleaseAction(lvlRgnAction_BC); 
    }if(rgn == M0){
      if(settingsConfig.EML){
        lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_M0); //Run Mesh Collection only
      }
    }if(rgn == M1){
      if(settingsConfig.EML){
        lvlscrnRgns[rgn].setReleaseAction(lvlscrnRgns_M1); //Run Mesh Collection using config settings
      }
    }
  }
}

void setLevelScreenText(){
    
  if(apigetPrintStats()){
    String tmp = String(api.printerStats.printerBedTempActual);
    
    String tempDisplayBed = tmp.substring(0,tmp.indexOf("."));
    tempDisplayBed += "/";
    tmp = String(api.printerStats.printerBedTempTarget);
    
    tempDisplayBed += tmp.substring(0,tmp.indexOf("."));
    lvlscrnRgns[BCV].displayText(&tft,tempDisplayBed,TFT_BLACK,TFT_YELLOW); //set text on screen for temp values of bed
  }
  
  MarkScreen();
}


//paints Red/Yellow dots on unusable controls. 
void MarkScreen(){
  if(needsHoming()){
    motionDisabled = true;
    tft.fillCircle(lvlscrnRgns[HXYZ].region.x+(lvlscrnRgns[HXYZ].region.w/2),lvlscrnRgns[HXYZ].region.y+(lvlscrnRgns[HXYZ].region.h/2)-5,6,TFT_YELLOW);
    tft.fillCircle(lvlscrnRgns[BLF].region.x+(lvlscrnRgns[BLF].region.w/2),lvlscrnRgns[BLF].region.y+(lvlscrnRgns[BLF].region.h/2),3,TFT_RED);
    tft.fillCircle(lvlscrnRgns[BRF].region.x+(lvlscrnRgns[BRF].region.w/2),lvlscrnRgns[BRF].region.y+(lvlscrnRgns[BRF].region.h/2),3,TFT_RED);
    tft.fillCircle(lvlscrnRgns[BLB].region.x+(lvlscrnRgns[BLB].region.w/2),lvlscrnRgns[BLB].region.y+(lvlscrnRgns[BLB].region.h/2),3,TFT_RED);
    tft.fillCircle(lvlscrnRgns[BRB].region.x+(lvlscrnRgns[BRB].region.w/2),lvlscrnRgns[BRB].region.y+(lvlscrnRgns[BRB].region.h/2),3,TFT_RED);
    tft.fillCircle(lvlscrnRgns[BC].region.x+(lvlscrnRgns[BC].region.w/2),lvlscrnRgns[BC].region.y+(lvlscrnRgns[BC].region.h/2),3,TFT_RED);
  }else{
    motionDisabled = false;
    tft.fillCircle(lvlscrnRgns[HXYZ].region.x+(lvlscrnRgns[HXYZ].region.w/2),lvlscrnRgns[HXYZ].region.y+(lvlscrnRgns[HXYZ].region.h/2)-5,6,TFT_BLACK); //this should remove the red dot if you home manually
    tft.fillCircle(lvlscrnRgns[BLF].region.x+(lvlscrnRgns[BLF].region.w/2),lvlscrnRgns[BLF].region.y+(lvlscrnRgns[BLF].region.h/2),3,TFT_BLACK);
    tft.fillCircle(lvlscrnRgns[BRF].region.x+(lvlscrnRgns[BRF].region.w/2),lvlscrnRgns[BRF].region.y+(lvlscrnRgns[BRF].region.h/2),3,TFT_BLACK);
    tft.fillCircle(lvlscrnRgns[BLB].region.x+(lvlscrnRgns[BLB].region.w/2),lvlscrnRgns[BLB].region.y+(lvlscrnRgns[BLB].region.h/2),3,TFT_BLACK);
    tft.fillCircle(lvlscrnRgns[BRB].region.x+(lvlscrnRgns[BRB].region.w/2),lvlscrnRgns[BRB].region.y+(lvlscrnRgns[BRB].region.h/2),3,TFT_BLACK);
    tft.fillCircle(lvlscrnRgns[BC].region.x+(lvlscrnRgns[BC].region.w/2),lvlscrnRgns[BC].region.y+(lvlscrnRgns[BC].region.h/2),3,TFT_BLACK);
  }
}


bool needsHoming(){
  getAxisPostionInfo();
  return axisInfo.Z == ZMAX;
}




//String lvlGCodePath;
void RunMeshCollection(bool useSettings){
    if(!runningMeshCollection){ //keeps it from triggering differently or again based on bounce or double tap.
      if (api.printJob.printerState != "Printing"){
        runningMeshCollection = true;
        mcUseSettings = useSettings;
        apigetPrintJob(); //make sure we have a current set of values
        previousPath = api.printJob.jobFilePath;
        #ifdef _debugopra
        Serial.print("PrevPath: ");
        Serial.println(previousPath);
        #endif 
        
        String lvlGCodePath(settingsConfig.OPLGCF);
        api.octoPrintFileSelect("/"+lvlGCodePath);
        api.octoPrintJobStart();
      }
    }
}
