#include "Arduino.h"

void blinkLED20(void){

  int i=0;

  for (i=0;i<10;i++){
  digitalWrite(20, HIGH);   // turn the LED off
  delay(50);               // wait

  digitalWrite(20, LOW);
  delay(50);
  }

}

