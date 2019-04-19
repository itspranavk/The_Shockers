#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

void setup() {
  
  #ifndef ESP8266 //What is this, in magsensor
  while (!Serial);
  #endif //What is this, in magsensor
  Serial.begin(9600);
  setupMag();
  setupAccel();
}

void loop() {
  Serial.print(getFanSpeedMultiplier(readTemp()));
  Serial.print(getVectorMag());
  Serial.print(getVectorAccel());
  delay(500);
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

void setupMag() {
  mag.enableAutoRange(true);
  if(!mag.begin())
  {
    Serial.println("No LSM303 detected. Check the wiring!");
    while(1);
  }
}

void setupAccel() {
  if(!accel.begin())
  {
    Serial.println("No LSM303 detected. Check the wiring!");
    while(1);
  }
}

int getVectorMag() {
  sensors_event_t event;
  mag.getEvent(&event);
  int vectors[3] = {event.magnetic.x, event.magnetic.y, event.magnetic.z};
  return vectors;
}

int getVectorAccel() {
  sensors_event_t event;
  accel.getEvent(&event);
  int vectors[3] = {event.acceleration.x, event.acceleration.y, event.acceleration.z};
  return vectors;  
}
