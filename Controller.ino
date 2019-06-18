#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

float unitTime;
float spd;
float dist;

struct vect {
  float arr[3];
};

void setup() {
  
  #ifndef ESP8266 //What is this, in magsensor
  while (!Serial);
  #endif //What is this, in magsensor
  Serial.begin(9600);
  setupMag();
  setupAccel();
  unitTime= (float)1 / (float)(16 * 10^6);
  spd= 0;
  dist= 0;
}

void loop() {
  Serial.print(getFanSpeedMultiplier(readTemp()));
  vect vAcc= getVectorAccel();
  vect vMag= getVectorMagne();
  
  Serial.print(vMag.arr[0]);
  Serial.print(vMag.arr[1]);
  Serial.print(vMag.arr[2]);
  Serial.print(vAcc.arr[0]);
  Serial.print(vAcc.arr[1]);
  Serial.print(vAcc.arr[2]);
  
  float magAcc= getMagAccel(vAcc);
  spd+= magAcc*unitTime;
  dist+= spd*unitTime/(float)2;

  float heading= getHeading(vMag);
  
  Serial.print(spd);
  Serial.print(dist);
  Serial.print(heading);
  
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

vect getVectorMagne() {
  sensors_event_t event;
  mag.getEvent(&event);
  vect v= {{event.magnetic.x, event.magnetic.y, event.magnetic.z}};
  return v;
}

vect getVectorAccel() {
  sensors_event_t event;
  accel.getEvent(&event);
  vect v= {{event.acceleration.x, event.acceleration.y, event.acceleration.z}};
  return v;  
}

float getMagAccel(vect vAcc) {
  float x= vAcc.arr[0];
  float y= vAcc.arr[1];
  float z= vAcc.arr[2] - 9.8;
  return sqrt(x*x + y*y + z*z);
}

float getHeading(vect vMag) {
  float heading= (atan2(vMag.arr[1], vMag.arr[0]) * 180) / 3.14159;
  if (heading < 0)
  {
    heading+= 360;
  }
  return heading;
}
