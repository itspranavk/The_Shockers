void setup() {
  Serial.begin(9600);
}

void loop() {
  int temp = readTemp();
  double fanSpeedMultiplier = getFanSpeedMultiplier(temp);
  Serial.print(fanSpeedMultiplier);
  delay(10);
}

int readTemp() {
  int temp = 20;
  return temp;
}

double getFanSpeedMultiplier(int temp) {
  if (temp < 20) {
    return 0;
  } else if (temp < 40) {
    return 0 + 0.02 * (40 - temp); 
  } else if (temp < 55) {
    return 0.4 + 0.04 * (55 - temp);
  } else {
    return 1;
  }
}
