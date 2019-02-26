/* The_Shockers
 * Authors: Pranav Kulkarni, Yuzhao Heng, Ohsung Kwon
 * 
 * Arduino sketch for A.James Clark School of Engineering's
 * 125 Mile E-Bike Challenge
 */

/* TMP36 Pin definitions */
const int TMP_PIN1 = 0; 
const int TMP_PIN2 = 1;
const int TMP_PIN3 = 2;

/* TMP36 Voltage Multiplier */
const double TMP_VOLT_MULTIPLIER = 5.0;

/* Fan Voltage Multiplier (variable) */
double FAN_VOLT_MULTIPLIER = 3.3;

/* Debug Mode (Set to false by default) */
bool isDebug = false;

/* Returns fan multiplier using defined fan curve */
double getFanMultiplier(double temp) {
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

/* Returns fan speed in volts */
double getFanSpeed(double temp) {
  return getFanMultiplier(temp) * FAN_VOLT_MULTIPLIER; 
}

/* Returns temperature */
double getTemperature(int pin) {
  int input = analogRead(pin);

  double volt = input * TMP_VOLT_MULTIPLIER;
  volt = volt / 1024.0;

  double tmp = (volt - 0.5) * 100;

  if (isDebug) {
    Serial.print("TMP36 Debug: ");
    Serial.print(volt); 
    Serial.print(" volts | ");
    Serial.print(tmp);
    Serial.println(" c");
  }

  return tmp;
}

/* Default setup() */
void setup() {
  Serial.begin(9600);
}

/* Default loop() */
void loop() {
  /* DO: Stuff */
  
  delay(1000);
}
