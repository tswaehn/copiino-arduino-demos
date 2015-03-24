/*
  Blink
  Turns on and off all LEDs 
 
  This example code is in the public domain.

*/


// this is a demo for how to import libs
// blinkLED20() comes from the test.cpp lib
#include "test.h"

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led0 = 13;
int led1 = 21; // additional CoPiino LED
int led2 = 20; // additional CoPiino LED

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);        
}

// the loop routine runs over and over again forever:
void loop() {
  int t=100;

  digitalWrite(led2, HIGH);   // turn the LED off
  delay(t);               // wait
  digitalWrite(led1, HIGH);   // turn the LED off
  delay(t);               // wait
  digitalWrite(led0, LOW);   // turn the LED on
  delay(2*t);               // wait
  
  digitalWrite(led0, HIGH);    // turn the LED on
  delay(t);               // wait
  digitalWrite(led1, LOW);    // turn the LED on
  delay(t);               // wait
  digitalWrite(led2, LOW);    // turn the LED on
  delay(2*t);               // wait
  
  // call "library"
  blinkLED20();

}



