/* Home Screen */
#define ctlMnu 0
#define motorStop 1
#define emergencyStop 2
#define startPrint 3
#define pausePrint 4
#define stopPrint 5
#define infoScreen 6 //also status
#define extruderTemps 7
#define bedTemps 8
#define stateTxt 9
#define fileTxt 10
#define uploadTime 11
#define timeEst 12
#define printTime 13
#define timeLeft 14
 

#define hmscrnRgnsSize 14
ActionRegion hmscrnRgns[hmscrnRgnsSize]; //home screen 




bool hmRgnPushed(int x, int y){
  
  for (uint8_t rgn = 0; rgn < hmscrnRgnsSize; rgn++) {   
    if(hmscrnRgns[rgn].contains(x,y,true)){
      #ifdef _debugTouch
      Serial.print("TouchPoint (");
      Serial.print(rgn); //finished test event text assertion
      Serial.print(" at:[x:");
      Serial.print(x);
      Serial.print("]/[y:");
      Serial.print(y);
      Serial.println("]");
      delay(1000);
      #endif
      return true;
    }
  }
  return false;
}


/*
 * Nav to Axis Control Screen
 */
void hmscrnRgns_Ctl(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_Ctl");
  #endif
  currentScreen = 1;
  initControlScreen();
}

/*
 * Nav to Control Menu Screen
 */
void hmscrnRgns_CtlMnu(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_CtlMnu");
  #endif
  currentScreen = 5;
  initCtlMnu();
}


/*
 * Nav to Temperature control screen
 */
void hmscrnRgns_Temp(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_Temp");
  #endif
  currentScreen = 2;
  initTempScreen();
}

void hmscrnRgns_QkStp(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_QkStp");
  #endif
  api.octoPrintPrinterCommand("M410"); //this is close to a true estop in that it is not really recoverable from. 
}

void hmscrnRgns_Info(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_info");
  #endif
  currentScreen = 4;
  initInfoScreen();
}



void hmscrnRgns_Print(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_Print");
  #endif 
  
  apigetPrintStats();
  if(api.printJob.jobFileName != ""){
    if(api.printerStats.printerStatePrinting){
      hmActionStatus = "Already Printing, Nothing to start";
    }else if(api.printerStats.printerStatepaused){
      #ifdef _debugopra
      Serial.println("Resuming Print");
      #endif 
      if(api.octoPrintJobResume()){hmActionStatus = "Resuming";}else{hmActionStatus = "Restart Command Failed";}
    //}else if(api.printerLastState.printerStateready){// naming
    }else if(api.printerStats.printerStateready){
      #ifdef _debugopra
      Serial.println("Starting Print");
      #endif
      if(api.octoPrintJobStart()){hmActionStatus = "Starting";}else{hmActionStatus = "Start Command Failed";}
    }else{
      hmActionStatus = "Start Command Failed";
    }
    ClearActionStatus.set(5000ul);
    setHomeText(false);
  }// it should already says "No File Loaded"
}


void hmscrnRgns_Pause(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_Pause");
  #endif
  if(api.printJob.jobFileName != ""){
    if(!api.printerStats.printerStatePrinting){
        hmActionStatus = "Not Printing, Nothing to Pause"; 
    }else if(api.octoPrintJobPause()){
      hmActionStatus = "Pausing";
    }else{
      hmActionStatus = "Pause Command Failed";
    }
    ClearActionStatus.set(5000ul);
    setHomeText(false);
  }// it should already says "No File Loaded"
}

void hmscrnRgns_Stop(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_Stop");
  #endif
  if(api.printJob.jobFileName != ""){
    if(!api.printerStats.printerStatePrinting){
      hmActionStatus = "Not Printing, Nothing to stop";
    }else if(api.octoPrintJobCancel()){
      hmActionStatus = "Cancelling";
    }else{
      hmActionStatus = "Stop/Cancel Command Failed";
    }
    ClearActionStatus.set(5000ul);
    setHomeText(false);
  } // it should already says "No File Loaded"
  
}

void hmscrnRgns_EStop(){
  #ifdef _debugopra
  Serial.println("hmscrnRgns_EStop");
  #endif
  api.octoPrintPrinterCommand("M112"); //this is close to a true estop in that it is not really recoverable from. 
}

bool loadHomeScreenRgns(){
   const char* sourceFile = "/hscrn.json";
  
  bool fileExists = SPIFFS.exists(sourceFile);
    
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

    char tmpSRgn[13];   
    //Control menu
    strlcpy(tmpSRgn, doc["C"] | "111111111111", 13);
    Region tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[ctlMnu].TextRgn = false;
    hmscrnRgns[ctlMnu].init(tmpRgn);

    //Stop Motors(rapid stop)
    strlcpy(tmpSRgn, doc["MS"] | "111111111111", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[motorStop].TextRgn = false;
    hmscrnRgns[motorStop].init(tmpRgn);

    //Emergency Stop
    strlcpy(tmpSRgn, doc["ES"] | "111111111111", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[emergencyStop].TextRgn = false;
    hmscrnRgns[emergencyStop].init(tmpRgn);    

    //Start Printing
    strlcpy(tmpSRgn, doc["P"] | "111111111111", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[startPrint].TextRgn = false;
    hmscrnRgns[startPrint].init(tmpRgn);    

    //pausePrint
    strlcpy(tmpSRgn, doc["PP"] | "111111111111", 13);
     tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[pausePrint].TextRgn = false;
    hmscrnRgns[pausePrint].init(tmpRgn);   
    
    //stopPrint
    strlcpy(tmpSRgn, doc["PS"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[stopPrint].TextRgn = false;
    hmscrnRgns[stopPrint].init(tmpRgn);   


    //infoScreen
    strlcpy(tmpSRgn, doc["I"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[infoScreen].TextRgn = false;
    hmscrnRgns[infoScreen].init(tmpRgn);   

    //extruderTemps
    strlcpy(tmpSRgn, doc["E"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[extruderTemps].TextRgn = true;
    hmscrnRgns[extruderTemps].init(tmpRgn);   

    //bedTemps
    strlcpy(tmpSRgn, doc["B"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[bedTemps].TextRgn = true;
    hmscrnRgns[bedTemps].init(tmpRgn);   

    //stateTxt
    strlcpy(tmpSRgn, doc["S"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[stateTxt].TextRgn = true;
    hmscrnRgns[stateTxt].init(tmpRgn);  

    //time
    strlcpy(tmpSRgn, doc["F"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[fileTxt].TextRgn = true;
    hmscrnRgns[fileTxt].init(tmpRgn);  

    //uploadTime
    strlcpy(tmpSRgn, doc["UF"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[uploadTime].TextRgn = true;
    hmscrnRgns[uploadTime].init(tmpRgn);  

    //timeEst
    strlcpy(tmpSRgn, doc["TE"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[timeEst].TextRgn = true;
    hmscrnRgns[timeEst].init(tmpRgn);  

    //printTime //time spent
    strlcpy(tmpSRgn, doc["TS"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[printTime].TextRgn = true;
    hmscrnRgns[printTime].init(tmpRgn);  

    //timeLeft    
    strlcpy(tmpSRgn, doc["TL"] | "111111111111", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    hmscrnRgns[timeLeft].TextRgn = true;
    hmscrnRgns[timeLeft].init(tmpRgn);  
    
    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < hmscrnRgnsSize; rgn++) {   
      Serial.println(hmscrnRgns[rgn].getRegionDebug());
      hmscrnRgns[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  
  
  return true;
}





void setHomeText(bool forceUpdate){
  
  #ifdef _debugopra
  Serial.println("updateHomeScreen text");
  #endif
  
  if(!forceUpdate && !apigetPrintStats()){return;} //if the data did not load, no need to update. 
  
  if(api.httpStatusCode !=200){
    Serial.print("OP API httpStatusCode(getPrinterStatistics):[");
    Serial.print(api.httpStatusCode);
    Serial.println("]");
    Serial.println("OP API httpErrorBody(getPrinterStatistics):[");
    Serial.print(api.httpErrorBody);
    Serial.println("]");
    
  }
  
  apigetPrintJob();
  if(api.httpStatusCode !=200){
    Serial.print("OP API httpStatusCode(getPrintJob):[");
    Serial.print(api.httpStatusCode);
    Serial.println("]");
    Serial.println("OP API httpErrorBody(getPrintJob):[");
    Serial.print(api.httpErrorBody);
    Serial.println("]");
  }
   
  
  ActionRegion aRgn[3];
  //tool0 temps
  String tmp = String(api.printerStats.printerTool0TempActual);
  
  String tempDisplayTool0 = "E: ";
  tempDisplayTool0 += tmp.substring(0,tmp.indexOf("."));
  tempDisplayTool0 += "/";
  
  tmp = String(api.printerStats.printerTool0TempTarget);
  
  tempDisplayTool0 += tmp.substring(0,tmp.indexOf("."));
 
  tmp = String(api.printerStats.printerBedTempActual);
  String tempDisplayBed = "B: ";
  tempDisplayBed += tmp.substring(0,tmp.indexOf("."));
  tempDisplayBed += "/";
  tmp = String(api.printerStats.printerBedTempTarget);
  tempDisplayBed += tmp.substring(0,tmp.indexOf("."));

  tft.setTextFont(2);
  tft.setTextSize(1);
  
  tft.fillRect(1, 1, 240,hmscrnRgns[extruderTemps].region.h, TFT_BLACK); //clear the top hmTxtRgns[7].h pixels
  
  hmscrnRgns[extruderTemps].displayText(&tft,tempDisplayTool0,TFT_BLACK,TFT_YELLOW); //set text on screen for temp values of tool0
  hmscrnRgns[bedTemps].displayText(&tft,tempDisplayBed,TFT_BLACK,TFT_YELLOW); //set text on screen for temp values of bed
    
  //tft.setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
  //tft.setTextColor(TFT_YELLOW);     // Set the font colour
  
  tft.fillRect(hmscrnRgns[stateTxt].region.x, hmscrnRgns[stateTxt].region.y, hmscrnRgns[stateTxt].region.w, hmscrnRgns[stateTxt].region.h, TFT_BLACK);
  hmscrnRgns[stateTxt].displayText(&tft,api.printerStats.printerState,TFT_BLACK,TFT_YELLOW);
  
  #ifdef _debugopra
  hmscrnRgns[stateTxt].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
  #endif
  
  if(api.printJob.printerState != "Offline" && api.printJob.jobFileName != ""){
    hmscrnRgns[fileTxt].displayText(&tft,api.printJob.jobFileName,TFT_BLACK,TFT_YELLOW);
  }else{
    hmscrnRgns[fileTxt].displayText(&tft,"          ---          ",TFT_BLACK,TFT_YELLOW);
  }
  
  #ifdef _debugopra
    hmscrnRgns[fileTxt].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
  #endif
  
  char bufDate[31];
  if(api.printJob.printerState != "Offline" && api.printJob.jobFileName != ""){
    //File upload time in human readable date and time YYYY-MM-DD HH:MM:SS
    sprintf(bufDate,"%04d-%02d-%02d %02d:%02d:%02d",
    year(api.printJob.jobFileDate),
    month(api.printJob.jobFileDate),
    day(api.printJob.jobFileDate),
    hour(api.printJob.jobFileDate),
    minute(api.printJob.jobFileDate),
    second(api.printJob.jobFileDate)
    );
  }else{
    sprintf(bufDate,"        ---        ");  
  }
  
  hmscrnRgns[uploadTime].displayText(&tft,bufDate,TFT_BLACK,TFT_YELLOW);
  
  #ifdef _debugopra
  hmscrnRgns[uploadTime].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
  #endif

  char estbuf[31];
  if(api.printJob.printerState != "Offline" && api.printJob.jobFileName != ""){
    //Estimated print time if availble in human readable time HH:MM:SS
    int estHours= api.printJob.estimatedPrintTime/3600;
    int estsecsRemaining=api.printJob.estimatedPrintTime%3600;
    int estMinutes=estsecsRemaining/60;
    int estSeconds=estsecsRemaining%60;
    sprintf(estbuf,"%02d:%02d:%02d",estHours,estMinutes,estSeconds);
  }else{
    sprintf(estbuf,"   ---   ");  
  }

  hmscrnRgns[timeEst].displayText(&tft,estbuf,TFT_BLACK,TFT_YELLOW);
  #ifdef _debugopra
  hmscrnRgns[timeEst].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button 
  #endif
  
  char pgrbuf[31];
  if(api.printJob.printerState != "Offline" && api.printJob.jobFileName != ""){
    int pgrHours= api.printJob.progressPrintTime/3600;
    int pgrsecsRemaining=api.printJob.progressPrintTime%3600;
    int pgrMinutes=pgrsecsRemaining/60;
    int pgrSeconds=pgrsecsRemaining%60;
    sprintf(pgrbuf,"%02d:%02d:%02d",pgrHours,pgrMinutes,pgrSeconds);
  }else{
    sprintf(pgrbuf,"   ---   ");  
  }


  hmscrnRgns[printTime].displayText(&tft,pgrbuf,TFT_BLACK,TFT_YELLOW);
  #ifdef _debugopra
  hmscrnRgns[printTime].drawRegion(&tft,TFT_YELLOW);
  #endif

  char buf[31];
  if(api.printJob.printerState != "Offline" && api.printJob.jobFileName != ""){
    //Print time left (if printing) in human readable time HH:MM:SS
    int runHours= api.printJob.progressPrintTimeLeft/3600;
    int secsRemaining=api.printJob.progressPrintTimeLeft%3600;
    int runMinutes=secsRemaining/60;
    int runSeconds=secsRemaining%60;
    sprintf(buf,"%02d:%02d:%02d",runHours,runMinutes,runSeconds);
  }else{
    sprintf(buf,"   ---   "); 
  }
  hmscrnRgns[timeLeft].displayText(&tft,buf,TFT_BLACK,TFT_YELLOW);
  #ifdef _debugopra
  hmscrnRgns[timeLeft].drawRegion(&tft,TFT_YELLOW);
  #endif

  if (ClearActionStatus.check()){
    if(api.printJob.printerState != "Offline" && api.printJob.jobFileName != ""){
      hmActionStatus = "Ready";
    }else if(api.printJob.printerState != "Offline" && api.printJob.jobFileName == ""){
      hmActionStatus = "No File Loaded";  
    }else{
      hmActionStatus = "     ---     ";
    }
  }
  
  
  hmscrnRgns[infoScreen].displayText(&tft,hmActionStatus,TFT_BLACK,TFT_YELLOW);
  #ifdef _debugopra
  hmscrnRgns[infoScreen].drawRegion(&tft,TFT_YELLOW);
  #endif
}
  
void initHomeScreen(){
  #ifdef _debugopra
  Serial.println("Running initHomeScreen, Screen 0");
  #endif
  //create regions for actions
  hmActionStatus = "Ready";
  ClearActionStatus.set(1ul);  
  tft.fillScreen(TFT_BLACK);            // Clear screen
  drawBmp("/Home.bmp", 0, 0);

  loadHomeScreenRgns();
  setHomeText(true);
  
  for (uint8_t rgn = 0; rgn < hmscrnRgnsSize; rgn++) { 
    //Serial.print("init AR:");
    //Serial.println(rgn);
    //hmscrnRgns[rgn].init(hmBtnRgn[rgn].x,hmBtnRgn[rgn].y,hmBtnRgn[rgn].w,hmBtnRgn[rgn].h);
    
    
    #ifdef _debugopra
    hmscrnRgns[rgn].drawRegion(&tft,TFT_YELLOW); // turn this on to debug visually the regions. draws a box around the button
    #endif
    if(rgn == 0){//0
      //hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_Ctl);
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_CtlMnu);
      
    }else if(rgn == 1){//1
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_QkStp); //motor stop
    }else if(rgn == 2){//2
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_EStop);
    }else if(rgn == 3){//3
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_Print);
    }else if(rgn == 4){//4
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_Pause);
    }else if(rgn == 5){
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_Stop);
    }else if(rgn == 6 || rgn == 7){
      hmscrnRgns[rgn].setReleaseAction(hmscrnRgns_Info);
    }

  }
}


/*
 * END Home Screen
 */
