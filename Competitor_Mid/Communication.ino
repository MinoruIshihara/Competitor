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

void sendDistance(int distance){
  if(distance > 31)distance = 31.0;
  unsigned char mapedDistance = (unsigned char)map(distance, 0, 31.0, 0, 255);
  Serial.write(mapedDistance);
}

void sendRadian(double radian){
  unsigned char mapedRadian = (unsigned char)map(radian * 100, -PI * 100, PI * 100, 0, 255);
  Serial.write(mapedRadian);
}

void sendPower(int left, int right){
  Serial.write((unsigned char)map(left, -400, 400, 0, 255));
  Serial.write((unsigned char)map(right, -400, 400, 0, 255));
}

void sendMode(int mode){
  Serial.write((unsigned char)mode);
}

void sendAll(RGB_STRUCT rgb, int distance, double radian, int right, int left, int mode){
  Serial.write('H');
  sendRGB(rgb);
  sendDistance(distance);
  sendRadian(radian);
  sendPower(left, right);
  sendMode(mode);
  Serial.write(0x0A);
}

void playSound(int soundNum){
  switch(soundNum){
    case 0:
    buzzer.playNote(NOTE_E(5), 1000, 15);
    buzzer.playNote(SILENT_NOTE, 25, 15);
    buzzer.playNote(NOTE_E(5), 100, 15);
    break;

    case 1:
    buzzer.playNote(NOTE_E(5), 1000, 15);
    buzzer.playNote(SILENT_NOTE, 250, 15);
    buzzer.playNote(NOTE_G(5), 1000, 15);
    buzzer.playNote(SILENT_NOTE, 250, 15);
    buzzer.playNote(NOTE_E(5), 1000, 15);
    break;
    
  }
}
