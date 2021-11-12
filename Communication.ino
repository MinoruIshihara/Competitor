void sendRGB(struct RGB_STRUCT rgb){
  unsigned int rgbBits = 0;
  rgbBits = rgbBits | (unsigned int)rgb.b;
  rgbBits = rgbBits | ((unsigned int)rgb.g << 4);
  rgbBits = rgbBits | ((unsigned int)rgb.r << 8);
  
  Serial.write(rgbBits);
}

void sendColor(struct RGB_STRUCT rgb){
//  Serial.write();
}

void sendDistance(int distance){
  Serial.write((char)(compenDistance(distance)));
}

void sendRadian(double mx, double my){
  Serial.write((unsigned char)mapRatian(calcRadian(mx, my)));
  Serial.println();
  Serial.println(mapRatian(calcRadian(mx, my)));
}

void sendPosition(struct RGB_STRUCT rgb){
  unsigned int rgbBits = 0;
  rgbBits = rgbBits | (unsigned int)rgb.b;
  rgbBits = rgbBits | ((unsigned int)rgb.g << 4);
  rgbBits = rgbBits | ((unsigned int)rgb.r << 8);
  
  Serial.write(rgbBits);
  Serial.println();
  Serial.println(rgbBits, BIN);
}
