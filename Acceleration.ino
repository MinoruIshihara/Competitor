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
  double deltaT = (double)(currentTime - pos.prevTime)/1000.0;

  compass.read();
  double ax = map(compass.a.x, -15800.0, 15800.0, -9800, 9800);
  double ay = map(compass.a.y, -16500.0, 16100.0, -9800, 9800);
  if(abs(ax) < 200)ax = 0.0;
  if(abs(ay) < 200)ay = 0.0;
  if(abs(pos.x + deltaT * ax) < 2550)pos.x += deltaT * ax;
  if(abs(pos.y + deltaT * ay) < 2550)pos.y += deltaT * ay;
  pos.prevTime = currentTime;
}

struct POSITION_STRUCT getPos(){
  struct POSITION_STRUCT mapedPos;
  mapedPos.prevTime = pos.prevTime;
  mapedPos.x = map((int)pos.x, -2550, 2550, 0, 255);
  mapedPos.y = map((int)pos.y, -2550, 2550, 0, 255);
  return mapedPos;
}
