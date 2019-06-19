#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_NeoPixel.h>

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

#define LED_PIN   6
#define LED_COUNT 4
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

float unitTime;
float spdPrev;
float spdCurr;
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
  
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  
  unitTime= (float)1 / (float)(16 * 10^6);
  spdCurr= 0;
  spdPrev= 0;
  dist= 0;
}

void loop() {
  Serial.print(getFanSpeedMultiplier(readTemp()));
  vect vAcc= getVectorAccel();
  vect vMag= getVectorMagne();
  
  setLightColor(0, strip.Color(255,   0,   0));
  setLightColor(1, strip.Color(0, 255, 0));
  setLightColor(2, strip.Color(0, 0, 255));
  setLightColor(3, strip.Color(255, 255, 255));
  
  Serial.print(vMag.arr[0]);
  Serial.print(vMag.arr[1]);
  Serial.print(vMag.arr[2]);
  Serial.print(vAcc.arr[0]);
  Serial.print(vAcc.arr[1]);
  Serial.print(vAcc.arr[2]);
  
  float magAcc= getMagAccel(vAcc);
  spdCurr= spdPrev + magAcc*unitTime;
  dist+= (spdPrev+spdCurr)*unitTime/(float)2;

  float heading= getHeading(vMag);
  
  Serial.print(spdCurr);
  Serial.print(dist);
  Serial.print(heading);

  spdPrev= spdCurr;
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

void setLightColor(int iPixel, uint32_t color) {
  strip.setPixelColor(iPixel, color);
  strip.show();
}
