#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define reed A0//pin connected to read switch
#define NUM_TEMP_SENSOR 3

//storage variables
int reedVal;
long timer;// time between one full rotation (ms)
float mph;
float mphPrev = 0;
float radius = 13.5;// tire radius (inches)
float circumference;
float dist = 0; // in miles 
float deltaDist = 0;
long countRotation = 0;

int tolerenceVal = 100;//min time (in ms) of one rotation (for debouncing)
int reedCounter;

//temp. variables
float temp;
//The pins for {Back, Bat2, Bat3}
int pins[] = {A4, A3, A2};

void setup(){
  
  reedCounter = tolerenceVal;
  circumference = 2*3.14*radius;
  pinMode(reed, INPUT);
  
  // more info at http://arduino.cc/playground/Code/Timer1 and 
  // https://arduinodiy.wordpress.com/2012/02/28/timer-interrupts/
  cli();//stop interrupts

  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;
  // set timer count for 1khz increments
  OCR1A = 1999;// = (1/1000) / ((1/(16*10^6))*8) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();//allow interrupts
  //END TIMER SETUP

  // begins serial port 
  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

ISR(TIMER1_COMPA_vect) {//Interrupt at freq of 1kHz to measure reed switch
  reedVal = digitalRead(reed);//get val of A0
  
  if (reedVal){//if reed switch is closed
    if (reedCounter == 0){//min time between pulses has passed
      mph = (56.8*float(circumference))/float(timer);//calculate miles per hour
      timer = 0;//reset timer
      reedCounter = tolerenceVal;//reset reedCounter
    }
    else{
      if (reedCounter > 0){//don't let reedCounter go negative
        reedCounter -= 1;//decrement reedCounter
      }
    }
    countRotation++;
    dist+= circumference;
    deltaDist += circumference;
  }
  else{//if reed switch is open
    if (reedCounter > 0){//don't let reedCounter go negative
      reedCounter -= 1;//decrement reedCounter
    }
  }
  if (timer > 2000){
    mph = 0;//if no new pulses from reed switch- tire is still, set mph to 0
  }
  else{
    timer += 1;//increment timer
  } 
}

void displayMPH(){
  // Print a message to the LCD.
  lcd.print("S:");
  Serial.println(mph);
  lcd.setCursor(2, 0);
  lcd.print((int)mph);
  
  lcd.setCursor(4, 0);
  lcd.print("mph");
}

void getDist() {
//  dist += (mph + mphPrev) / (3600 *2);
//  mphPrev = mph; // Updates every delay time 
  dist += circumference;
}

void displayDist() {
  lcd.setCursor(8, 0);
  lcd.print("D:");
  lcd.setCursor(10, 0);
  lcd.print((int)dist);
  lcd.setCursor(13, 0);
  lcd.print("m");
}

void displayTEMP(){
  lcd.setCursor(0, 1);
  lcd.print("T: ");
  for (int i = 0; i < NUM_TEMP_SENSOR; i++) {
    // read analog volt from sensor and save to variable temp
    float t = analogRead(pins[i]);
    t *= 0.52828125;
    Serial.print(t);
    Serial.println(" *F");
    lcd.setCursor(i*4 + 3, 1);
    lcd.print((int)t);
    lcd.setCursor(i*4 + 6, 1); // 3 digits given to 
    lcd.print("/");
  }
}

void loop(){
//  Display sample 
//  S:77mph D:101m_ 
//  T: 77 /61 /845/
  //print mph once a second
  displayMPH();
  getDist();
  // Update only when delta dist reaches 0.1mi
  if (deltaDist > 0.1) {
    displayDist();
    deltaDist = 0;
  }
  displayTEMP();
  delay(1000);
}
