#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define reed A0//pin connected to read switch

//storage variables
int reedVal;
long timer;// time between one full rotation (ms)
float mph;
float radius = 13.5;// tire radius (inches)
float circumference;

int tolerenceVal = 100;//min time (in ms) of one rotation (for debouncing)
int reedCounter;

//temp. variables
float temp;
int tempPin = A4;

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
  // Print a message to the LCD.
  lcd.print("Speed: ");
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
  Serial.println(mph);
  lcd.setCursor(7, 0);
  lcd.print(mph);
  
  lcd.setCursor(12, 0);
  lcd.print("mph");
}

void displayTEMP(){
  temp = analogRead(tempPin);
   // read analog volt from sensor and save to variable temp
  temp = temp * 0.52828125;
  Serial.println(temp);
  Serial.println(" *F");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.setCursor(6, 1);
  lcd.print(temp);
  lcd.setCursor(11, 1);
  lcd.print(" F");
}
void loop(){
  //print mph once a second
  displayMPH();
  displayTEMP();
  delay(1000);
}
