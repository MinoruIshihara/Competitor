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
  delay(60);
  double distance = (0.61 * TEMPERATURE + 331.5) * intervalTime * 0.5 / 10000;

  if(distance > 31.0 || 1.0 > distance )return 31.0;
  return distance;
}
