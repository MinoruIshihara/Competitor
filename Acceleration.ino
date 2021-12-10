void initAcceleration(){
}

struct ACCEL_STRUCT getAccel(){
  struct ACCEL_STRUCT accel;
  
  compass.read();
  accel.x = compass.a.x;
  accel.y = compass.a.y;
  
  return accel;
}

struct POSITION_STRUCT getPos(struct POSITION_STRUCT pos, int deltaT){
  pos.x +=  (double)deltaT / 1000.0 * motors.getRightSpeed();
  pos.y +=  (double)deltaT / 1000.0 * motors.getRightSpeed();
  if(abs(pos.x) > 2550)pos.x = 2550;
  if(abs(pos.y) > 2550)pos.y = 2550;
  
  #ifdef DEBUG_ACCEL
  Serial.print("speed: ");
  Serial.print(motors.getRightSpeed());
  Serial.print(", ");
  Serial.println(motors.getLeftSpeed());
  #endif
  
  return pos;
}
