bool serialEvent(){
  if(Serial.available() > 0){
    if(Serial.read() == 'H'){
      return true;
    }
  }
  return false;  
}

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
  if(distance > 31)distance = 31.0;
  unsigned char mapedDistance = (unsigned char)map(distance, 0, 31.0, 0, 255);
  Serial.write(mapedDistance);
}

void sendRadian(double radian){
  unsigned char mapedRadian = (unsigned char)map(radian * 100, -PI * 100, PI * 100, 0, 255);
  Serial.write(mapedRadian);
}

void sendPos(struct POSITION_STRUCT pos){
  struct POSITION_STRUCT mapedPos = {0, 0};
  mapedPos.x = map((int)pos.x, -2550, 2550, 0, 255);
  mapedPos.y = map((int)pos.y, -2550, 2550, 0, 255);
  Serial.write((unsigned char)mapedPos.x);
  Serial.write((unsigned char)mapedPos.y);
}

void sendAll(RGB_STRUCT rgb, int distance, double radian, struct POSITION_STRUCT mpos){
  Serial.write('H');
  sendRGB(rgb);
  sendDistance(distance);
  sendRadian(radian);
  sendPos(mpos);
  Serial.write(0x0A);
}
