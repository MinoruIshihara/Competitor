void sendRGB(struct RGB_STRUCT rgb){
  unsigned char rBits = rgb.r >> 4;
  unsigned char gBits = rgb.g >> 4;
  unsigned char bBits = rgb.b >> 4;
  Serial.write(rBits);
  Serial.write((gBits << 4)| bBits);
}

void sendColor(struct RGB_STRUCT rgb){
//  Serial.write();
}

void sendDistance(int distance){
  unsigned char mapedDistance = (unsigned char)map(distance, 0, 31.0, 0, 255);
  Serial.write(mapedDistance);
}

void sendRadian(double radian){
  unsigned char mapedRadian = (unsigned char)map(radian, -PI, PI, 0, 255);
  Serial.write(mapedRadian);
}

void sendPos(struct POSITION_STRUCT mpos){
  Serial.write((unsigned char)mpos.x);
  Serial.write((unsigned char)mpos.y);
}
