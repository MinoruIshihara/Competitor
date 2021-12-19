const int TRIG_PIN = 2;
const int ECHO_PIN = 4;
const float TEMPERATURE = 16.0;

int initSonar(){
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

double getDistance(){
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int intervalTime = pulseIn(ECHO_PIN, HIGH, 23071);
  delay(30);
  double distance = (0.61 * TEMPERATURE + 331.5) * intervalTime * 0.5 / 10000;

  if(distance > 150.0 || 2.5 > distance )distance = 150.0;

  #ifdef DEBUG_SONOR
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.print(", ");
  #endif
  
  return distance;
}
