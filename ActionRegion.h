#include <TFT_eSPI.h>
#define _debugAR

typedef void (*actionCallback)(void);
static void dummyButtonAction(void) { }; // In case user callback is not defined!
struct Region{int16_t x; int16_t y; int16_t w; int16_t h;};

Region getRegionFromString(char numbers[13]){
    //0010010650 5  0
    //0123456789 10 11
    Region rgn;
    char x[4]; char y[4];  char w[4];  char h[4];  
    for(int i=0;i<3;i++){ x[i] = numbers[i];} x[3] = '\0';
    for(int i=0;i<3;i++){ y[i] = numbers[i+3];} y[3] = '\0';
    for(int i=0;i<3;i++){ w[i] = numbers[i+6];} w[3] = '\0';
    for(int i=0;i<3;i++){ h[i] = numbers[i+9];} h[3] = '\0';
    rgn.x = atoi(x); rgn.y = atoi(y); rgn.w = atoi(w); rgn.h = atoi(h);
        
    #ifdef _debugopra
    Serial.print("Origenal value:");Serial.println(numbers);
    Serial.print("Region x:");Serial.println(rgn.x);
    Serial.print("Region y:");Serial.println(rgn.y);
    Serial.print("Region w:");Serial.println(rgn.w);
    Serial.print("Region h:");Serial.println(rgn.h);
    #endif
    
    return rgn;
}





class ActionRegion {
 public:
  ActionRegion(void);
  void     init(int16_t x, int16_t y,uint16_t w, uint16_t h);
  void     init(int16_t x, int16_t y,uint16_t w, uint16_t h,bool isTextRgn);
  void     init(Region rgn);
  void     init(Region rgn,bool isTextRgn);
  bool     contains(int16_t x, int16_t y,bool updateState);
  void     press(bool p);
  bool     isPressed();
  bool     TextRgn;
  bool     justPressed();
  bool     justReleased();
  void     setX(int16_t x);
  void     setY(int16_t y);
  void     setH(int16_t h);
  void     setW(int16_t w);
  //region   getRegion(); 
  String   getRegionDebug();
  void     displayText(TFT_eSPI *tft,String text,uint16_t bgColor,uint16_t tColor,uint16_t tDatum); 
  actionCallback pressAction   = dummyButtonAction; // Press   action callback
  actionCallback releaseAction = dummyButtonAction; // Release action callback
  void     setPressAction(actionCallback action);
  void     setReleaseAction(actionCallback action);
  void     drawRegion(TFT_eSPI *tft,int16_t color);
  void     setPressTime(uint32_t pressTime)    { _pressTime  = pressTime; }
  void     setReleaseTime(uint32_t releaseTime){ _releaseTime = releaseTime; }
  uint32_t getPressTime(void)  { return _pressTime; }
  uint32_t getReleaseTime(void){ return _releaseTime; }
  bool     getState(void) {return _currstate;}        // Get inverted state, true = inverted
  Region   region; //internal region
  
 private:
  
  uint32_t _pressTime, _releaseTime;
  bool _currstate, _laststate; // region states
};
//******************************************************************************************************

void     ActionRegion::setX(int16_t x){
  region.x = x;
}
void     ActionRegion::setY(int16_t y){
  region.y = y;
}
void     ActionRegion::setH(int16_t h){
  region.h = h;
}
void     ActionRegion::setW(int16_t w){
  region.w = w;
}



ActionRegion::ActionRegion(void) {
  region.x = 0;region.y=0;region.w = 0;region.h = 0;
  _currstate = false;
  _laststate = false;
}

//TL_DATUM = 0 = Top left, TC_DATUM = 1 = Top centre, TR_DATUM = 2 = Top right, ML_DATUM = 3 = Middle left, MC_DATUM = 4 = Middle centre, MR_DATUM = 5 = Middle right, BL_DATUM = 6 = Bottom left, BC_DATUM = 7 = Bottom centre, BR_DATUM = 8 = Bottom right
void ActionRegion::displayText(TFT_eSPI *tft, String text,uint16_t bgColor,uint16_t tColor = TFT_YELLOW,uint16_t Datum = MC_DATUM){
  tft->setTextDatum(Datum);        // Use center(middle) center as text coord datum
  tft->setTextColor(tColor);     // Set the font colour

  tft->fillRect(region.x,region.y,region.w,region.h,bgColor);
  tft->drawString(text,region.x+(region.w/2),region.y+(region.h/2));
  
}



String ActionRegion::getRegionDebug(void){
  String debug = "{";
  debug += String(region.x);
  debug += ",";
  debug += String(region.y);
  debug += ",";
  debug += String(region.w);
  debug += ",";
  debug += String(region.h);
  debug += "}";
  return debug;
}
void ActionRegion::setPressAction(actionCallback action)
{
  pressAction = action;
}

void ActionRegion::setReleaseAction(actionCallback action)
{
  releaseAction = action;
}
void ActionRegion::init(Region rgn){init(rgn,false);}
void ActionRegion::init(Region rgn,bool isTextRgn){
  region = rgn;TextRgn = isTextRgn;
  _pressTime=0xFFFFFFFF;
  _releaseTime=0xFFFFFFFF;  
}
void ActionRegion::init(int16_t x, int16_t y, uint16_t w, uint16_t h){Region rgn {x,y,w,h};init(rgn,false);}
void ActionRegion::init(int16_t x, int16_t y, uint16_t w, uint16_t h,bool isTextRgn){Region rgn {x,y,w,h};init(rgn,isTextRgn);}

bool ActionRegion::contains(int16_t x, int16_t y,bool updateState = false) {
  if(region.x == 0){
    #ifdef _debugAR
    Serial.print("Error: Action regon has a zero as a value [contains method aborted]");
    #endif
    return false;
  }
  bool rtn = ((x >= region.x) && (x < (region.x + region.w)) && (y >= region.y) && (y < (region.y + region.h)));

  if(updateState){
    press(rtn);
  }
  return rtn;
}

void ActionRegion::press(bool p) {
  _laststate = _currstate;
  _currstate = p;
  if(justPressed()){
    pressAction();
  }else if(justReleased()){
    releaseAction();
  }
}

bool ActionRegion::isPressed()    { return _currstate; }
bool ActionRegion::justPressed()  { return (_currstate && !_laststate); }
bool ActionRegion::justReleased() { return (!_currstate && _laststate); }
void ActionRegion::drawRegion(TFT_eSPI *tft,int16_t color = TFT_YELLOW){
 
  if(TextRgn){
    //draw x at center of region
    tft->drawFastVLine(region.x+(region.w/2), region.y,region.h , color);
    tft->drawFastHLine(region.x, region.y+(region.h/2), region.w, color);
    //tft->drawRect(region.x-(region.w/2), region.y-(region.h/2), region.w, region.h, color);
  }
    tft->drawRect(region.x, region.y, region.w, region.h, color); 
  
}
