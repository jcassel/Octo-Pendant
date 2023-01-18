
#define tmpscrnRgsSize 27
ActionRegion tmpscrnRgs[tmpscrnRgsSize];

//#define returnRgn 0 already defined.
#define E1 1
#define E5 2
#define E10 3
#define E20 4
#define EPO 5
#define EPlus 6
#define ET 7
#define EMinus 8
#define EST 9
#define E170 10
#define E180 11
#define E200 12
#define E230 13
#define B1 14
#define B5 15
#define B10 16
#define B20 17
#define BPO 18
#define BPlus 19
#define BT 20
#define BMinus 21
#define BST 22
#define B50 23
#define B60 24
#define B70 25
#define B100 26
#define ECV 27
#define BCV 28

 
bool tmpRgnPushed(int x, int y){
  for (uint8_t rgn = 0; rgn < tmpscrnRgsSize; rgn++) {   
    if(tmpscrnRgs[rgn].contains(x,y,true)){
      #ifdef _debugopra
      Serial.print("pushed tmpscrnRgs[");
      Serial.print(rgn); //finished test event text assertion
      Serial.println("]");
      #endif
      return true;
    }else{
      #ifdef _debugopra
      Serial.print("Checked tmpscrnRgs[");
      Serial.print(rgn); //finished test event text assertion
      Serial.println("]");
      #endif
    }
  }
  return false;
}

int16_t Tool0SetValue = 0;
int16_t Tool0SetValueIncrement = 0;
int16_t BedSetValue = 0;
int16_t BedSetValueIncrement = 0;

void tmpscrnRgs0(){
  #ifdef _debugopra
  Serial.println("tmpscrnRgs_0");
  #endif
  currentScreen = 5;
  initCtlMnu();
}
void tmpscrnRgs1(){
  #ifdef _debugopra
  Serial.println("tmpscrnRgs_1");
  #endif
  //set tool text to "1";
  Tool0SetValueIncrement = 1;
}
void tmpscrnRgs2(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("2");
  Serial.println("before Tool0SetValue: " + String(Tool0SetValue));
  #endif
  Tool0SetValueIncrement = 5;
  #ifdef _debugopra
  Serial.println("After Tool0SetValue: " + String(Tool0SetValue));
  #endif
  
}
void tmpscrnRgs3(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("3");
  #endif
  Tool0SetValueIncrement = 10;
}

void tmpscrnRgs4(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("4");
  #endif
  Tool0SetValueIncrement = 20;
}

void tmpscrnRgs5(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("5");
  #endif
  api.octoPrintSetTool0Temperature(0);
  setTemperatureScreenText();
}

void tmpscrnRgs6(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("6");
  Serial.println("Before Tool0SetValue: " + String(Tool0SetValue));
  #endif
  Tool0SetValue =  Tool0SetValue + Tool0SetValueIncrement;
  if (Tool0SetValue > 260){Tool0SetValue = 260;}
  #ifdef _debugopra
  Serial.println("After Tool0SetValue: " + String(Tool0SetValue));
  #endif
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW);
}

//26 needed
void tmpscrnRgs7(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("7");
  #endif 
}

void tmpscrnRgs8(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("8");
  Serial.println("Before Tool0SetValue: " + String(Tool0SetValue));
  #endif
  Tool0SetValue =  Tool0SetValue - Tool0SetValueIncrement;
  if (Tool0SetValue < 0){Tool0SetValue = 0;}
  #ifdef _debugopra
  Serial.println("After Tool0SetValue: " + String(Tool0SetValue));
  #endif
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW);
}


void tmpscrnRgs9(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("9");
  #endif
  api.octoPrintSetTool0Temperature(Tool0SetValue);
  setTemperatureScreenText();
}

void tmpscrnRgs10(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("10");
  #endif
  Tool0SetValue = 170;
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW);
  
  
}
void tmpscrnRgs11(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("11");
  #endif
  Tool0SetValue = 180;
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs12(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("12");
  #endif
  Tool0SetValue = 200;
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs13(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("13");
  #endif
  Tool0SetValue = 230;
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs14(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("14");
  #endif
  BedSetValueIncrement  =1;
}
void tmpscrnRgs15(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("15");
  #endif
  BedSetValueIncrement = 5;
}

void tmpscrnRgs16(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("16");
  BedSetValueIncrement = 10;
  #endif
}
void tmpscrnRgs17(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("17");
  #endif
  BedSetValueIncrement = 20;
}

void tmpscrnRgs18(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("18");
  #endif
  api.octoPrintSetBedTemperature(0);
  setTemperatureScreenText();
}
void tmpscrnRgs19(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("19");
  Serial.println("Before Tool0SetValue: " + String(Tool0SetValue));
  #endif
  BedSetValue =  BedSetValue + BedSetValueIncrement;
  if (BedSetValue > 140 ){BedSetValue = 140;}
  Serial.println("After Tool0SetValue: " + String(BedSetValue));
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs20(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("20");
  #endif
}
void tmpscrnRgs21(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("21");
  Serial.println("Before Tool0SetValue: " + String(Tool0SetValue));
  #endif
  BedSetValue =  BedSetValue - BedSetValueIncrement;
  if (BedSetValue < 0){BedSetValue = 0;}
  #ifdef _debugopra
  Serial.println("After Tool0SetValue: " + String(BedSetValue));
  #endif
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs22(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("22");
  #endif
  api.octoPrintSetBedTemperature(BedSetValue);
  setTemperatureScreenText();
}
void tmpscrnRgs23(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("23");
  #endif
  BedSetValue = 50;
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs24(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("24");
  #endif
  BedSetValue = 60;
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW);
}
void tmpscrnRgs25(){
  #ifdef _debugopra
  Serial.print("tmpscrnRgs_");
  Serial.println("25");
  #endif
  BedSetValue = 70;
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW);  
}
void tmpscrnRgs26(){
  #ifdef _debugopra
  Serial.print("*tmpscrnRgs_");
  Serial.println("26");
  #endif 
  BedSetValue = 100;
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW);
}

bool loadTempScreenRgns(){
  const char* sourceFile = "/tmpscrn.json";
  
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
  tmpscrnRgs[returnRgn].TextRgn = false;
  tmpscrnRgs[returnRgn].init(tmpRgn);
  
    //E1
    strlcpy(tmpSRgn, doc["E1"] | "010050050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E1].TextRgn = false;
    tmpscrnRgs[E1].init(tmpRgn);    

    //E5
    strlcpy(tmpSRgn, doc["E5"] | "068050050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E5].TextRgn = false;
    tmpscrnRgs[E5].init(tmpRgn);    

    //E10
    strlcpy(tmpSRgn, doc["E10"] | "123050052033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E10].TextRgn = false;
    tmpscrnRgs[E10].init(tmpRgn);    

    //E20
    strlcpy(tmpSRgn, doc["E20"] | "181050050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E20].TextRgn = false;
    tmpscrnRgs[E20].init(tmpRgn);    

    //EPO
    strlcpy(tmpSRgn, doc["EPO"] | "015087035033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[EPO].TextRgn = false;
    tmpscrnRgs[EPO].init(tmpRgn);    

    //E+
    strlcpy(tmpSRgn, doc["E+"] | "051087033033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[EPlus].TextRgn = false;
    tmpscrnRgs[EPlus].init(tmpRgn);    

    //ET
    strlcpy(tmpSRgn, doc["ET"] | "086087045033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[ET].TextRgn = true;
    tmpscrnRgs[ET].init(tmpRgn);    

    //E-
    strlcpy(tmpSRgn, doc["E-"] | "132087033033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[EMinus].TextRgn = false;
    tmpscrnRgs[EMinus].init(tmpRgn);    

    //EST
    strlcpy(tmpSRgn, doc["EST"] | "193087040033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[EST].TextRgn = false;
    tmpscrnRgs[EST].init(tmpRgn);    

    //E170
    strlcpy(tmpSRgn, doc["E170"] | "010130050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E170].TextRgn = false;
    tmpscrnRgs[E170].init(tmpRgn);    

    //E180
    strlcpy(tmpSRgn, doc["E180"] | "068130050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E180].TextRgn = false;
    tmpscrnRgs[E180].init(tmpRgn);    

    //E200
    strlcpy(tmpSRgn, doc["E200"] | "123130050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E200].TextRgn = false;
    tmpscrnRgs[E200].init(tmpRgn);    

    //E230
    strlcpy(tmpSRgn, doc["E230"] | "181130050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[E230].TextRgn = false;
    tmpscrnRgs[E230].init(tmpRgn);    

    //B1
    strlcpy(tmpSRgn, doc["B1"] | "010206050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B1].TextRgn = false;
    tmpscrnRgs[B1].init(tmpRgn);    

    //B5
    strlcpy(tmpSRgn, doc["B5"] | "071206050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B5].TextRgn = false;
    tmpscrnRgs[B5].init(tmpRgn);    

    //B10
    strlcpy(tmpSRgn, doc["B10"] | "124206050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B10].TextRgn = false;
    tmpscrnRgs[B10].init(tmpRgn);    

    //B20
    strlcpy(tmpSRgn, doc["B20"] | "181206050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B20].TextRgn = false;
    tmpscrnRgs[B20].init(tmpRgn);    

    //BPO
    strlcpy(tmpSRgn, doc["BPO"] | "010240045035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[BPO].TextRgn = false;
    tmpscrnRgs[BPO].init(tmpRgn);    

    //B+
    strlcpy(tmpSRgn, doc["B+"] | "055240030035", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[BPlus].TextRgn = false;
    tmpscrnRgs[BPlus].init(tmpRgn);    

    //BT
    strlcpy(tmpSRgn, doc["BT"] | "086240045033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[BT].TextRgn = true;
    tmpscrnRgs[BT].init(tmpRgn);    

    //B-
    strlcpy(tmpSRgn, doc["B-"] | "132240033033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[BMinus].TextRgn = false;
    tmpscrnRgs[BMinus].init(tmpRgn);    

    //BST
    strlcpy(tmpSRgn, doc["BST"] | "193240040033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[BST].TextRgn = false;
    tmpscrnRgs[BST].init(tmpRgn);    
    
    //B50
    strlcpy(tmpSRgn, doc["B50"] | "010282050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B50].TextRgn = false;
    tmpscrnRgs[B50].init(tmpRgn);    

    //B60
    strlcpy(tmpSRgn, doc["B60"] | "067282050033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B60].TextRgn = false;
    tmpscrnRgs[B60].init(tmpRgn);    

    //B70
    strlcpy(tmpSRgn, doc["B70"] | "123282052033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B70].TextRgn = false;
    tmpscrnRgs[B70].init(tmpRgn);    

    //B100
    strlcpy(tmpSRgn, doc["B100"] | "179282052033", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[B100].TextRgn = false;
    tmpscrnRgs[B100].init(tmpRgn);    

    //ECV
    strlcpy(tmpSRgn, doc["ECV"] | "162015060020", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[ECV].TextRgn = true;
    tmpscrnRgs[ECV].init(tmpRgn);    

    //BCV
    strlcpy(tmpSRgn, doc["BCV"] | "162175060020", 13);
    tmpRgn = getRegionFromString(tmpSRgn);
    tmpscrnRgs[BCV].TextRgn = true;
    tmpscrnRgs[BCV].init(tmpRgn);    

    #ifdef _debugopra    
    for (uint8_t rgn = 0; rgn < tmpscrnRgsSize; rgn++) {   
      Serial.println(tmpscrnRgs[rgn].getRegionDebug());
      tmpscrnRgs[rgn].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    }
    #endif
    configfile.close();
  
  
  return true;
}


void initTempScreen(){
  #ifdef _debugopra
  Serial.println("Running initTempScreen, Screen 2");
  #endif
  
  tft.fillScreen(TFT_BLACK);            // Clear screen
  drawBmp("/Temps.bmp", 0, 0); //no bmp for this screen.  
  loadTempScreenRgns();
    
  tmpscrnRgs[returnRgn].setReleaseAction(tmpscrnRgs0);
  
  tmpscrnRgs[E1].setReleaseAction(tmpscrnRgs1);
  
  tmpscrnRgs[E5].setReleaseAction(tmpscrnRgs2);
  
  tmpscrnRgs[E10].setReleaseAction(tmpscrnRgs3);
  
  tmpscrnRgs[E20].setReleaseAction(tmpscrnRgs4);
  
  tmpscrnRgs[EPO].setReleaseAction(tmpscrnRgs5);
  
  tmpscrnRgs[EPlus].setReleaseAction(tmpscrnRgs6);
  
  tmpscrnRgs[ET].setReleaseAction(tmpscrnRgs7);
  
  tmpscrnRgs[EMinus].setReleaseAction(tmpscrnRgs8);
  
  tmpscrnRgs[EST].setReleaseAction(tmpscrnRgs9);
  
  tmpscrnRgs[E170].setReleaseAction(tmpscrnRgs10);
  
  tmpscrnRgs[E180].setReleaseAction(tmpscrnRgs11);
  
  tmpscrnRgs[E200].setReleaseAction(tmpscrnRgs12);
  
  tmpscrnRgs[E230].setReleaseAction(tmpscrnRgs13);
  
  tmpscrnRgs[B1].setReleaseAction(tmpscrnRgs14);
  
  tmpscrnRgs[B5].setReleaseAction(tmpscrnRgs15);
  tmpscrnRgs[B10].setReleaseAction(tmpscrnRgs16);
  
  tmpscrnRgs[B20].setReleaseAction(tmpscrnRgs17);
  tmpscrnRgs[BPO].setReleaseAction(tmpscrnRgs18);
  tmpscrnRgs[BPlus].setReleaseAction(tmpscrnRgs19);
  
  tmpscrnRgs[BT].setReleaseAction(tmpscrnRgs20);
  
  tmpscrnRgs[BMinus].setReleaseAction(tmpscrnRgs21);
  
  tmpscrnRgs[BST].setReleaseAction(tmpscrnRgs22);
  
  tmpscrnRgs[B50].setReleaseAction(tmpscrnRgs23);
  
  tmpscrnRgs[B60].setReleaseAction(tmpscrnRgs24);
  
  tmpscrnRgs[B70].setReleaseAction(tmpscrnRgs25);
  
  tmpscrnRgs[B100].setReleaseAction(tmpscrnRgs26);
  
  setTemperatureScreenText();
}

void setTemperatureScreenText(){
  //nothing to do here yet. 
  tft.setTextFont(2);
  tft.setTextSize(1);
  //api.getPrinterState();
  apigetPrintStats();

  //String tmp = String(api.printerLastState.printerTool0TempActual);//better naming would be nice
  String tmp = String(api.printerStats.printerTool0TempActual);
  
  String tempDisplayTool0 = tmp.substring(0,tmp.indexOf("."));
  tempDisplayTool0 += "/";
  //tmp = String(api.printerLastState.printerTool0TempTarget);//better naming would be nice
  tmp = String(api.printerStats.printerTool0TempTarget);
  
  tempDisplayTool0 += tmp.substring(0,tmp.indexOf("."));
  tmpscrnRgs[ECV].displayText(&tft,tempDisplayTool0,TFT_BLACK,TFT_YELLOW); //set text on screen for temp values of tool0
  tmpscrnRgs[ET].displayText(&tft,String(Tool0SetValue),TFT_BLACK,TFT_YELLOW); //Update the text on screen for the Set value related to Tool0
  
  
  //tmp = String(api.printerLastState.printerBedTempActual); //better naming would be nice
  tmp = String(api.printerStats.printerBedTempActual);
  
  String tempDisplayBed = tmp.substring(0,tmp.indexOf("."));
  tempDisplayBed += "/";
  //tmp = String(api.printerLastState.printerBedTempTarget); //better naming would be nice
  tmp = String(api.printerStats.printerBedTempTarget);
  
  tempDisplayBed += tmp.substring(0,tmp.indexOf("."));
  tmpscrnRgs[BCV].displayText(&tft,tempDisplayBed,TFT_BLACK,TFT_YELLOW); //set text on screen for temp values of bed
  tmpscrnRgs[BT].displayText(&tft,String(BedSetValue),TFT_BLACK,TFT_YELLOW); //Update the text on screen for the Set value related to Tool0
  
  #ifdef _debugopra
    tmpscrnRgs[ECV].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    tmpscrnRgs[ET].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    tmpscrnRgs[BCV].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
    tmpscrnRgs[BT].drawRegion(&tft,TFT_YELLOW); //Draws a box around the region
  #endif
}
