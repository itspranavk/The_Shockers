#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

#define LED_PIN   6
#define LED_COUNT 8
#define TCAADDR 0x70
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

struct vect {
  float x;
  float y;
  float z;
};

float unitTime;
float spdPrev;
float spdCurr;
float spdCurrAbs;
float dist;
const int PORT_TEMP_1 = 0;
const int PORT_TEMP_2 = 1;
const int PORT_TEMP_3 = 2;

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
  spdCurr = 0;
  spdPrev= 0;
  dist= 0;
}

void loop() {
//  int temp1 = readTemp(PORT_TEMP_1);
//  int temp2 = readTemp(PORT_TEMP_2);
//  int temp3 = readTemp(PORT_TEMP_3);
  
//  Serial.print(getFanSpeedMultiplier(temp1));
  vect vAcc= getVectorAccel();
  vect vMag= getVectorMagne();
  
//  setLightColor(0, strip.Color(255,   0,   0));
//  setLightColor(1, strip.Color(0, 255, 0));
//  setLightColor(2, strip.Color(0, 0, 255));
//  setLightColor(3, strip.Color(255, 255, 255));
//  
//  setLightColor(4, strip.Color(255,   0,   0));
//  setLightColor(5, strip.Color(0, 255, 0));
//  setLightColor(6, strip.Color(0, 0, 255));
//  setLightColor(7, strip.Color(255, 255, 255));
  
//  Serial.print("Vector magnetometer: "); 
//  Serial.print(vMag.x); Serial.print(vMag.y); Serial.print(vMag.z);
//  Serial.println();
//  Serial.print("vector acclerometer: ");
//  Serial.print(vAcc.x); Serial.print(vAcc.y); Serial.print(vAcc.z);
//  Serial.println();

  //float error = getMagAccel(vAcc) * unitTime;

    float magAcc = getMagAccel(vAcc) - 9.81;

    if (magAcc < 0.5) {
      magAcc = 0;
      spdPrev = 0;
    }

    if (vAcc.y < 0) {
      magAcc = -magAcc;
    }

    spdCurr = spdPrev + magAcc*unitTime;
  
//  spdCurr= spdPrev + magAcc*unitTime;
//  spdCurrAbs= abs(spdCurr);
 //dist+= abs((magSpd)*unitTime/(float)2);
//
//  float heading= getHeading(vMag);
//  String headingT= getHeadingText(heading);

   if (abs(spdCurr) > 0) { 
Serial.print("Current speed: "); Serial.print(spdCurr); Serial.print(" Accel: "); Serial.println(getMagAccel(vAcc));
   }
//  Serial.print("Current speed, absolute: "); Serial.println(spdCurr);
//  Serial.print("Distance traveled: "); Serial.println(dist);
//  Serial.print("Magnetic heading: "); Serial.println(heading);
//  Serial.print("Magnetic heading, text: "); Serial.println(headingT.c_str());

  spdPrev= spdCurr;
  //delay(500);
}

int readTemp(uint8_t i) {
  tcaselect(i);
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
  vect v= {event.magnetic.x, event.magnetic.y, event.magnetic.z};
  return v;
}

vect getVectorAccel() {
  sensors_event_t event;
  accel.getEvent(&event);
  vect v= {event.acceleration.x, event.acceleration.y, event.acceleration.z};
  return v;  
}

// A 1D vector 
float getMagAccel(vect vAcc) {
  float x= vAcc.x;
  float y= vAcc.y;
  float z= vAcc.z;
  float sumSquares= square(x) + square(y) + square(z);
  
  return sqrt(sumSquares);
}

// Can be negative 
float square(float x) {
  return x*x;
}

// Between 0, 360 
float getHeading(vect vMag) {
  float heading= (atan2(vMag.y, vMag.x) * 180) / 3.14159;
  if (heading < 0)
  {
    heading+= 360;
  }
  return heading;
}

String getHeadingText(float heading) {
  // unit= 45, half= 22.5 
  if (337.5 < heading || heading < 22.5) {
    return "N";
  } else if (22.5 < heading && heading < 67.5) {
    return "NE";
  } else if (67.5 < heading && heading < 112.5) {
    return "E";
  } else if (112.5 < heading && heading < 157.5) {
    return "SE";
  } else if (157.5 < heading && heading < 202.5) {
    return "S";
  } else if (202.5 < heading && heading < 247.5) {
    return "SW";
  } else if (247.5 < heading && heading < 292.5) {
    return "W";
  } else if (292.5 < heading && heading < 337.5) {
    return "NW";
  } else {
    return "ERROR";
  }
}

void setLightColor(int iPixel, uint32_t color) {
  strip.setPixelColor(iPixel, color);
  strip.show();
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}
