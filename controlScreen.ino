/*
 * Control Screen 
 */
#define ctlscrnRgnsSize 17
ActionRegion ctlscrnRgns[ctlscrnRgnsSize]; //control screen

#define CTLR    0
#define YU   1
#define ZU   2
#define XL  3
#define XYH  4
#define XR  5
#define ZH   6
#define YD  7
#define ZD  8
#define MVP1  9
#define MV1   10
#define MV10   11
#define MV20   12
#define FON   13
#define FOff  14
#define MOff  15
#define ctl_SV 16

int ctltxt = 100;

bool ctlRgnPushed(int x, int y){
  for (uint8_t rgn = 0; rgn < ctlscrnRgnsSize; rgn++) {   
    if(ctlscrnRgns[rgn].contains(x,y,true)){//true here causes the hooked event to fire if this return true. 
      #ifdef _debugopra
      Serial.println(rgn); //finished test event text assertion
      #endif
      return true;
    }
  }
  return false;
}


void setControlScreenText(){
  //ctl_SV
  //lvlscrnRgns[BCV].displayText(&tft,tempDisplayBed,TFT_BLACK,TFT_YELLOW); //set text on screen for temp values of bed
  
  //tft.fillRect(ctltxtRegion[0], ctltxtRegion[1], ctltxtRegion[3], ctltxtRegion[2], TFT_BLACK);
  //tft.setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
  //tft.setTextColor(TFT_YELLOW);     // Set the font colour
  // Draw the string, the value returned is the width in pixels
  #ifdef _debugopra
  Serial.print("ctlMvVal: ");
  Serial.println(ctltxt);
  #endif 
  
  String displayValue = String(ctltxt*0.1);
  if(ctltxt == 1){
    displayValue = "0.1";
  }else{
    displayValue = displayValue.substring(0,displayValue.length()-3);
  }

  ctlscrnRgns[ctl_SV].displayText(&tft,displayValue,TFT_BLACK,TFT_YELLOW);
  //ctltxtRegion[3] = tft.drawString(displayValue,ctltxtRegion[0], ctltxtRegion[1]);
  #ifdef _debugopra
  ctlscrnRgns[ctl_SV].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
  //tft.drawRect(ctltxtRegion[0], ctltxtRegion[1], ctltxtRegion[3], ctltxtRegion[2], TFT_YELLOW);
  #endif

  if(needsHoming()){
    motionDisabled = true;
    tft.fillCircle(ctlscrnRgns[XYH].region.x+(ctlscrnRgns[XYH].region.w/2),ctlscrnRgns[XYH].region.y+(ctlscrnRgns[XYH].region.h/2),6,TFT_YELLOW);
    tft.fillCircle(ctlscrnRgns[ZH].region.x+(ctlscrnRgns[ZH].region.w/2),ctlscrnRgns[ZH].region.y+(ctlscrnRgns[ZH].region.h/2),6,TFT_YELLOW);
    tft.fillCircle(ctlscrnRgns[ZU].region.x+(ctlscrnRgns[ZU].region.w/2),ctlscrnRgns[ZU].region.y+(ctlscrnRgns[ZU].region.h/2),3,TFT_RED);
    tft.fillCircle(ctlscrnRgns[ZD].region.x+(ctlscrnRgns[ZD].region.w/2),ctlscrnRgns[ZD].region.y+(ctlscrnRgns[ZD].region.h/2),3,TFT_RED);
    
    tft.fillCircle(ctlscrnRgns[YU].region.x+(ctlscrnRgns[YU].region.w/2),ctlscrnRgns[YU].region.y+(ctlscrnRgns[YU].region.h/2),3,TFT_RED);
    tft.fillCircle(ctlscrnRgns[YD].region.x+(ctlscrnRgns[YD].region.w/2),ctlscrnRgns[YD].region.y+(ctlscrnRgns[YD].region.h/2),3,TFT_RED);
    tft.fillCircle(ctlscrnRgns[XL].region.x+(ctlscrnRgns[XL].region.w/2),ctlscrnRgns[XL].region.y+(ctlscrnRgns[XL].region.h/2),3,TFT_RED);
    tft.fillCircle(ctlscrnRgns[XR].region.x+(ctlscrnRgns[XR].region.w/2),ctlscrnRgns[XR].region.y+(ctlscrnRgns[XR].region.h/2),3,TFT_RED);
  }else{
    motionDisabled = false;
    tft.fillCircle(ctlscrnRgns[XYH].region.x+(ctlscrnRgns[XYH].region.w/2),ctlscrnRgns[XYH].region.y+(ctlscrnRgns[XYH].region.h/2),6,TFT_BLACK); //this should remove the red dot if you home manually
    tft.fillCircle(ctlscrnRgns[ZH].region.x+(ctlscrnRgns[ZH].region.w/2),ctlscrnRgns[ZH].region.y+(ctlscrnRgns[ZH].region.h/2),6,TFT_BLACK); //this should remove the red dot if you home manually
    tft.fillCircle(ctlscrnRgns[ZU].region.x+(ctlscrnRgns[ZU].region.w/2),ctlscrnRgns[ZU].region.y+(ctlscrnRgns[ZU].region.h/2),3,TFT_BLACK);
    tft.fillCircle(ctlscrnRgns[ZD].region.x+(ctlscrnRgns[ZD].region.w/2),ctlscrnRgns[ZD].region.y+(ctlscrnRgns[ZD].region.h/2),3,TFT_BLACK);
    tft.fillCircle(ctlscrnRgns[YU].region.x+(ctlscrnRgns[YU].region.w/2),ctlscrnRgns[YU].region.y+(ctlscrnRgns[YU].region.h/2),3,TFT_BLACK);
    tft.fillCircle(ctlscrnRgns[YD].region.x+(ctlscrnRgns[YD].region.w/2),ctlscrnRgns[YD].region.y+(ctlscrnRgns[YD].region.h/2),3,TFT_BLACK);
    tft.fillCircle(ctlscrnRgns[XL].region.x+(ctlscrnRgns[XL].region.w/2),ctlscrnRgns[XL].region.y+(ctlscrnRgns[XL].region.h/2),3,TFT_BLACK);
    tft.fillCircle(ctlscrnRgns[XR].region.x+(ctlscrnRgns[XR].region.w/2),ctlscrnRgns[XR].region.y+(ctlscrnRgns[XR].region.h/2),3,TFT_BLACK);
  }


  
}


void ctlRgnAction_YU(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_YU");
  #endif
  if(!motionDisabled){
    api.octoPrintPrintHeadRelativeJog(0,(ctltxt*0.1),0,0);//x,y,z,f(speed)
  }
}

void ctlRgnAction_ZU(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_ZU");
  #endif
  if(!motionDisabled){
    api.octoPrintPrintHeadRelativeJog(0,0,(ctltxt*0.1),0);//x,y,z,f(speed)
  }
}

void ctlRgnAction_XL(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_XL");
  #endif
  if(!motionDisabled){
    api.octoPrintPrintHeadRelativeJog(((ctltxt*0.1)/-1),0,0,0);//x,y,z,f(speed)
  }
}

void ctlRgnAction_XYH(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_XYH");
  #endif 
  if(needsHoming()) {
    api.sendPostToOctoPrint("/api/printer/printhead","{\"command\": \"home\",\"axes\": [\"x\",\"y\",\"z\"]}");
  }else{
    api.octoPrintPrintHeadHome(); //<<< Named badly. This really homes the X and Y Axis not Z
  }
}

void ctlRgnAction_XR(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_XR");
  #endif 
  if(!motionDisabled){
    api.octoPrintPrintHeadRelativeJog((ctltxt*0.1),0,0,0);//x,y,z,f(speed)
  }
}
    
void ctlRgnAction_ZH(){
 #ifdef _debugopra
 Serial.println("ctlRgnAction_ZH"); 
 #endif
  
 if(needsHoming()) {
  api.sendPostToOctoPrint("/api/printer/printhead","{\"command\": \"home\",\"axes\": [\"x\",\"y\",\"z\"]}");
 }else{
  api.sendPostToOctoPrint("/api/printer/printhead","{\"command\": \"home\",\"axes\": [\"z\"]}"); 
 }
}
    
void ctlRgnAction_YD(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_YD");
  #endif
  if(!motionDisabled){
    api.octoPrintPrintHeadRelativeJog(0,((ctltxt*0.1)/-1),0,0);//x,y,z,f(speed)
  }
}
    
void ctlRgnAction_ZD(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_ZD");
  #endif
  if(!motionDisabled){
    api.octoPrintPrintHeadRelativeJog(0,0,((ctltxt*0.1)/-1),0);//x,y,z,f(speed)
  }
}
    
void ctlRgnAction_MVP1(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_MVP1");
  #endif
  ctltxt = 1; //recall this is really 0.1
  setControlScreenText();
}
    
void ctlRgnAction_MV1(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_MV1");
  #endif 
  ctltxt = 10; //recall this is really 1
  setControlScreenText();
}
    
void ctlRgnAction_MV10(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_MV10");
  #endif
  ctltxt = 100; //recall this is really 10
  setControlScreenText();
}
    
void ctlRgnAction_MV20(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_MV20");
  #endif
  ctltxt = 200; //recall this is really 20
  setControlScreenText();
}
    
void ctlRgnAction_FON(){    
  #ifdef _debugopra
  Serial.println("ctlRgnAction_FON");
  #endif
  //gcode command M106 [I<index>],[P<index>],[S<speed>],[T<Secondary>]
  api.octoPrintPrinterCommand("M106 S255");
}
void ctlRgnAction_FOff(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_FOff");
  #endif 
  //gcode command M107 [P<index>]
  api.octoPrintPrinterCommand("M107");
}
    
void ctlRgnAction_MOff(){ //turn motors off. 
  #ifdef _debugopra
  Serial.println("ctlRgnAction_MOff");
  #endif
  api.octoPrintPrinterCommand("M18");
}
   
   
void ctlRgnAction_CTLR(){
  #ifdef _debugopra
  Serial.println("ctlRgnAction_CTLR");
  #endif
  currentScreen = 5;
  initCtlMnu();
}


bool loadControlScreenRgns(){
  const char* sourceFile = "/infoscrn.json";
  
  bool fileExists = SPIFFS.exists(sourceFile);
  
  char tmpSRgn[13];   
  strlcpy(tmpSRgn, "001001045045", 13); //<< plug in the same value you would have from your file if you want to change this at this level.
  Region tmpRgn = getRegionFromString(tmpSRgn);
  ctlscrnRgns[CTLMR].TextRgn = false;
  ctlscrnRgns[CTLMR].init(tmpRgn);
    
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
    strlcpy(tmpSRgn, doc["CTLR"] | "001001045045", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[CTLMR].TextRgn = false;
    ctlscrnRgns[CTLMR].init(tmpRgn);
  
    //YU
    strlcpy(tmpSRgn, doc["YU"] | "061053050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[YU].TextRgn = false;
    ctlscrnRgns[YU].init(tmpRgn);    

    //ZU
    strlcpy(tmpSRgn, doc["ZU"] | "175053053040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[ZU].TextRgn = false;
    ctlscrnRgns[ZU].init(tmpRgn);   

    //XL
    strlcpy(tmpSRgn, doc["XL"] | "010100055045", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[XL].TextRgn = false;
    ctlscrnRgns[XL].init(tmpRgn);   

    //XYH
    strlcpy(tmpSRgn, doc["XYH"] | "061100050050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[XYH].TextRgn = false;
    ctlscrnRgns[XYH].init(tmpRgn);   
    
    //XR
    strlcpy(tmpSRgn, doc["XR"] | "108100055045", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[XR].TextRgn = false;
    ctlscrnRgns[XR].init(tmpRgn);       

    //ZH
    strlcpy(tmpSRgn, doc["ZH"] | "175100053050", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[ZH].TextRgn = false;
    ctlscrnRgns[ZH].init(tmpRgn);       

    //YD
    strlcpy(tmpSRgn, doc["YD"] | "061158050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[YD].TextRgn = false;
    ctlscrnRgns[YD].init(tmpRgn);       

    //ZD
    strlcpy(tmpSRgn, doc["ZD"] | "175156053040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[ZD].TextRgn = false;
    ctlscrnRgns[ZD].init(tmpRgn);       

    //MVP1
    strlcpy(tmpSRgn, doc["MVP1"] | "015205050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[MVP1].TextRgn = false;
    ctlscrnRgns[MVP1].init(tmpRgn);       

    //MV1
    strlcpy(tmpSRgn, doc["MV1"] | "070205050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[MV1].TextRgn = false;
    ctlscrnRgns[MV1].init(tmpRgn);       

    //MV10
    strlcpy(tmpSRgn, doc["MV10"] | "126205050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[MV10].TextRgn = false;
    ctlscrnRgns[MV10].init(tmpRgn);       

    //MV20
    strlcpy(tmpSRgn, doc["MV20"] | "180205050040", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[MV20].TextRgn = false;
    ctlscrnRgns[MV20].init(tmpRgn);       

    //FON
    strlcpy(tmpSRgn, doc["FON"] | "005265070045", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[FON].TextRgn = false;
    ctlscrnRgns[FON].init(tmpRgn);       

    //FOff
    strlcpy(tmpSRgn, doc["FOff"] | "085265070045", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[FOff].TextRgn = false;
    ctlscrnRgns[FOff].init(tmpRgn);       

    //MOff
    strlcpy(tmpSRgn, doc["MOff"] | "163265070045", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[MOff].TextRgn = false;
    ctlscrnRgns[MOff].init(tmpRgn);       

    //ctl_SV
    strlcpy(tmpSRgn, doc["ctl_SV"] | "135168020020", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    ctlscrnRgns[ctl_SV].TextRgn = true;
    ctlscrnRgns[ctl_SV].init(tmpRgn);       

    
    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < ctlscrnRgnsSize; rgn++) {   
      Serial.println(ctlscrnRgns[rgn].getRegionDebug());
      ctlscrnRgns[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  return true;
}


  
void initControlScreen(){
  #ifdef _debugopra
  Serial.println("Running initControlScreen, Screen 1");
  #endif

  tft.fillScreen(TFT_BLACK);            // Clear screen
  drawBmp("/Control1.bmp", 0, 0);
  loadControlScreenRgns();
  
  
  //create regions for actions  
  for (uint8_t rgn = 0; rgn < ctlscrnRgnsSize; rgn++) { 
    ctlscrnRgns[YU].setReleaseAction(ctlRgnAction_YU);
    ctlscrnRgns[ZU].setReleaseAction(ctlRgnAction_ZU);
    ctlscrnRgns[XL].setReleaseAction(ctlRgnAction_XL);
    ctlscrnRgns[XYH].setReleaseAction(ctlRgnAction_XYH);
    ctlscrnRgns[XR].setReleaseAction(ctlRgnAction_XR);
    ctlscrnRgns[ZH].setReleaseAction(ctlRgnAction_ZH);
    ctlscrnRgns[YD].setReleaseAction(ctlRgnAction_YD);
    ctlscrnRgns[ZD].setReleaseAction(ctlRgnAction_ZD);
    ctlscrnRgns[MVP1].setReleaseAction(ctlRgnAction_MVP1);
    ctlscrnRgns[MV1].setReleaseAction(ctlRgnAction_MV1);
    ctlscrnRgns[MV10].setReleaseAction(ctlRgnAction_MV10);
    ctlscrnRgns[MV20].setReleaseAction(ctlRgnAction_MV20);
    ctlscrnRgns[FON].setReleaseAction(ctlRgnAction_FON);
    ctlscrnRgns[FOff].setReleaseAction(ctlRgnAction_FOff);
    ctlscrnRgns[MOff].setReleaseAction(ctlRgnAction_MOff);
    ctlscrnRgns[CTLR].setReleaseAction(ctlRgnAction_CTLR);
    #ifdef _debugopra
      ctlscrnRgns[rgn].drawRegion(&tft); // turn this on to debug visually the regions. 
    #endif
    
  }

  setControlScreenText();
}
