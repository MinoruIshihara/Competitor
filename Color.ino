const unsigned int R_MAX = 969;
const unsigned int G_MAX = 1024;
const unsigned int B_MAX = 1024;
const unsigned int R_MIN = 81;
const unsigned int G_MIN = 120;
const unsigned int B_MIN = 126;

int initColorSensor(){
  tcs.begin();
}

struct RGB_STRUCT getRGB(){
  struct RGB_STRUCT rgb;
  unsigned int r, g, b, clr;
  tcs.getRawData(&r, &g, &b, &clr);
  rgb.r = map(r, R_MIN, R_MAX, 0, 255);
  rgb.g = map(g, G_MIN, G_MAX, 0, 255);
  rgb.b = map(b, B_MIN, B_MAX, 0, 255);

  return rgb;
}

void calibrationRGB(){
  static unsigned int rMax = 0;
  static unsigned int gMax = 0;
  static unsigned int bMax = 0;
  static unsigned int rMin = 30000;
  static unsigned int gMin = 30000;
  static unsigned int bMin = 30000;
  
  unsigned int r, g, b, clr;
  
  tcs.getRawData(&r, &g, &b, &clr);
  if(r > rMax)rMax = r;
  if(g > gMax)gMax = g;
  if(b > bMax)bMax = b;
  if(r < rMin)rMin = r;
  if(g < gMin)gMin = g;
  if(b < bMin)bMin = b;

  Serial.print("Max:");
  Serial.print(rMax);
  Serial.print(':');
  Serial.print(gMax);
  Serial.print(':');
  Serial.println(bMax);

  Serial.print("Min:");
  Serial.print(rMin);
  Serial.print(':');
  Serial.print(gMin);
  Serial.print(':');
  Serial.println(bMin);
}
