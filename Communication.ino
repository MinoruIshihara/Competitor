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
  unsigned char mapedDistance = (unsigned char)map(distance, 0, 31.0, 0, 255);
  Serial.write((unsigned char)distance);
  Serial.println();
  Serial.println(distance);
}

void sendRadian(double radian){
  unsigned char mapedRadian = (unsigned char)map(radian, -PI, PI, 0, 255);
  Serial.write(mapedRadian);
  Serial.println();
  Serial.println(mapedRadian);
}

void sendPos(struct POSITION_STRUCT pos){
  unsigned char mapedPosX = map((int)pos.x, -255, 255, 0, 255);
  unsigned char mapedPosY = map((int)pos.y, -255, 255, 0, 255);
  unsigned int xyBits = 0;
  xyBits = xyBits | (unsigned int)pos.y;
  xyBits = xyBits | ((unsigned int)pos.x <<8);
  
  Serial.write(xyBits);
  Serial.println();
  Serial.println(xyBits, BIN);
}
