#include <Adafruit_NeoPixel.h>

#define NUMPIXELS      6
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define COL1 pixels.Color(0,255,0)
#define COL2 pixels.Color(180,255,0)
#define COL3 pixels.Color(255,255,0)
#define COL4 pixels.Color(255,200,0)
#define COL5 pixels.Color(255,120,0)
#define COL6 pixels.Color(255,0,0)
uint32_t colors[] = {COL1, COL2, COL3, COL4, COL5, COL6};
#define DEFAULT_COLOR_RANGES "700 1000 1300 1600 1900 "
//uint16_t colorRanges[] = {700, 1000, 1300, 1600, 1900};
byte maxBrightness = 120;

byte ppm2idx(int ppm) {
  int rng;
  for (byte i=0; i < 5; i++) {
    EEPROM.get(EE_10B_TH + i*2, rng);
    if (ppm < rng) return i;
  }
  return 5;
}

uint32_t idx2color(byte idx) {
  return colors[idx];
}

void storeColorRanges(char *p) {
  String num;
  byte idx = 0;
  if (*p == 'd') storeColorRanges(DEFAULT_COLOR_RANGES);
  else {
    while (idx < 5) {
      if (isDigit(*p)) num += *p;
      else {
        EEPROM.put(EE_10B_TH + (idx++ * 2), num.toInt());
        num = "";
      }
      if (!*p) break;
      p++;
    }
    
  }
}



void initColorRanges() {
  int rng;
  EEPROM.get(EE_10B_TH, rng);
  if (rng == -1) storeColorRanges(DEFAULT_COLOR_RANGES);
}

void initNeopixels() {
  initColorRanges();
  pixels.begin();
  for (int i=0; i< 6; i++) pixels.setPixelColor(i, colors[i]);
  pixels.setBrightness(20);
  pixels.show();
}

void debugInfoNeopixel() {
  Serial << F("Brightness  :") << sBrightness<< endl;
}

void processBrightness() {
  if (overrideBrightness == 255) { 
    raLight.addValue(analogReadFine(LIGHT_PIN, 1));
    sBrightness = map((int)raLight.getAverage(), 0, 1024, 1, maxBrightness);
  } else {
    sBrightness = overrideBrightness;
  }
  pixels.setBrightness(sBrightness);
  pixels.show();
}

void processColors() {
  if (!sPPM) return;
  byte idx = ppm2idx(sPPM);
  uint32_t color = idx2color(idx);
  byte i = 0;
  for (; i <= idx     ; i++) pixels.setPixelColor(i, color);
  for (; i < NUMPIXELS; i++) pixels.setPixelColor(i, 0);
  pixels.show(); 
}

void overrideLedsMax() {
  int col = 5;
  for (int i=0; i < col   ; i++) pixels.setPixelColor(i, colors[col-1]);
  //pixels.setBrightness(200);
  pixels.show();
  
}

void processNeopixels() {
  if (overrideLeds) {
    overrideLedsMax();
       processBrightness();
  } else {
    processColors();
    processBrightness(); 
  }
}
