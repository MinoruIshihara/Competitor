void initPos(){
  pos.prevTime = millis();
  pos.x = 0;
  pos.y = 0;
}

struct ACCEL_STRUCT getAccel(){
  struct ACCEL_STRUCT accel;
  
  compass.read();
  accel.x = compass.a.x;
  accel.y = compass.a.y;
  
  return accel;
}

void setPos(){
  int currentTime = millis();
  
  compass.read();
  pos.x += (currentTime - pos.prevTime) * compass.a.x;
  pos.y += (currentTime - pos.prevTime) * compass.a.y;
  pos.prevTime = currentTime;
}
