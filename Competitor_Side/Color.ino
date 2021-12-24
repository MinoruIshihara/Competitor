#ifdef CALIBRATION
unsigned int rMax = 0;
unsigned int gMax = 0;
unsigned int bMax = 0;
unsigned int rMin = 30000;
unsigned int gMin = 30000;
unsigned int bMin = 30000;
#else
unsigned int rMax = 969;
unsigned int gMax = 1024;
unsigned int bMax = 1024;
unsigned int rMin = 81;
unsigned int gMin = 120;
unsigned int bMin = 126;
#endif

int initColorSensor(){
  tcs.begin();
  #ifdef CALIBRATION
    button.waitForButton();
    calibrationRGB();
  #endif
  playSound(0);
}

struct RGB_STRUCT getRGB(){
  struct RGB_STRUCT rgb;
  unsigned int r, g, b, clr;
  tcs.getRawData(&r, &g, &b, &clr);
  rgb.r = map(r, rMin, rMax, 0, 255);
  rgb.g = map(g, gMin, gMax, 0, 255);
  rgb.b = map(b, bMin, bMax, 0, 255);

  return rgb;
}

void calibrationRGB(){
  unsigned int startTime = millis();
  unsigned int r, g, b, clr;

  motors.setSpeeds(100, 100);
  while(millis() - startTime <= 2000){  
    tcs.getRawData(&r, &g, &b, &clr);
    rMax = max(r, rMax);
    gMax = max(g, gMax);
    bMax = max(b, bMax);
    rMin = min(r, rMin);
    gMin = min(g, gMin);
    bMin = min(b, bMin);
  }
  
  motors.setSpeeds(0, 0);
}

int identify_color(RGB_STRUCT rgb, RGB_STRUCT initRGB )
{
  float d2 = 
  pow(initRGB.r - rgb.r, 2) + 
  pow(initRGB.g - rgb.g, 2) + 
  pow(initRGB.b - rgb.b, 2);
  return ( d2 < 900 );
}
