/*
  Blink
  Turns on and off all LEDs 
 
  This example code is in the public domain.

*/

#include "math.h"
#include "Wire.h"
#include "HTU21D.h"
#include "HTU21D_softI2C.h"
#include "FanControl.h"
  
  
HTU21D outterSens;
HTU21Dsoft innerSens; 
FanControl fanControl(8, 50); // use Pin 8 for PWM output, default speed 60

// Pin 13 has an LED connected on most Arduino boards.
int led = 20; // additional CoPiino LED

// --- temperature control
#define HYST 2
#define OUTTER_MAX_T 22
#define INNER_MAX_T 18
#define INNER_MIN_T 7


char temperatureControl( char outterT, char innerT ){
 
  static char oldOutterT=-60;
  static char oldInnerT=-60;
  
  // if no temperature change of more then 2degree -> quit
  if ((abs(outterT-oldOutterT) < HYST) && (abs(innerT-oldInnerT) < HYST)){
   return -10*fanControl.getFanState();    
  }
  oldOutterT= outterT;
  oldInnerT= innerT;
  
  
  // check if outter temperature is too high
  if (outterT > OUTTER_MAX_T){
  	fanControl.switchOff();
    return fanControl.getFanState();
  }
  
  // if inner temperature is too low
  if (innerT < INNER_MIN_T){
  	fanControl.switchOff();
    return fanControl.getFanState();
  }
  
  // if inner temperature is too high
  if (innerT > INNER_MAX_T){
   	fanControl.switchOff();
    return fanControl.getFanState();
  }
  
  // else ... 
  fanControl.switchOn();
  
  //
  return fanControl.getFanState();
}


// ---

void execute( char * array[], unsigned char size ){

  static char i=0;
  char k;
  char buf[50];
  float fValue0, fValue1;
  int value0, value1;

  if (size == 0){
    return;
  }
  if (size > 10){
    return;
  }
  Serial1.println("cmd");

  for (k=0;k<size; k++){
    sprintf( buf, "%d: %s", k, array[k] );

    Serial1.println( buf );    

  }

  Serial1.println("done");

  //
  if (strcmp("speed", array[0] ) == 0){
    fValue0 = strtod( array[1], 0 );
		
    value0= (int) fValue0;

    fanControl.setSpeed( value0 );
    
    return;
  }
 
  
} 

/*
	default function for receiving via serial1
*/
void receiveCommand(){

  const int MAX_BUF=30;
  char incomingByte;
  static char buf[MAX_BUF];
  static char bufIdx=0;
  char i;
  char value;
  char * array[10];
  char size=0;
  char * pItem;

  // send data only when you receive data:
  while (Serial1.available() > 0) {

    // read the incoming byte:
    incomingByte = Serial1.read();
    
    if (incomingByte == 0x0A){


      // add space termination
      buf[bufIdx++]=0x20;

      pItem = &buf[0];
      for (i=0;i<bufIdx;i++){
        if (buf[i] == 0x20){
           buf[i]= 0x0;
          array[size++]= pItem;
          pItem= &buf[i+1];
        }

      }

      // evaluate buffer
      execute( array, size );

      // reset index
      bufIdx=0;
    } else {
      // put data into buffer
      if (bufIdx >= MAX_BUF){
        bufIdx=0;
      }

      buf[bufIdx++] = incomingByte;
    }

    // say what you got:
    // Serial1.write(incomingByte);

  }
}


// just blink some LED
void blink(){
  static char i=0;

  // invert i
  i= 1-i;
  
  // switch LED depending on i
  if (i){
	  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  }
}


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);  

  // opens serial port 1 to Raspberry, sets data rate to 115200 bps
  // serial port 0 is as usual the Arduino port
  Serial1.begin(115200);
  Serial1.println("- start program - ");  
  
  
  Wire.begin();
	outterSens.begin();  
  innerSens.begin();

  // setup PWM frequency
  TCCR2B &= ~0x07;
  TCCR2B |= 3;
  
  // enable fan control
  fanControl.begin();
  
}

// the loop routine runs over and over again forever:
void loop() {

  static unsigned long startTime=0;
  unsigned long currentTime;
  static char x=0;
  static char y=0;
  static char i=0;

  float outterHumd, innerHumd;
  float outterTemp, innerTemp;
  char outterRh, innerRh;
  char outterT, innerT;
  char fanState;
  unsigned char fanSpeed;
  
  // check for incomming commands
	receiveCommand();
    
  // process Fan control
  fanControl.process();
  
  // print every 100ms something
  currentTime= millis();
  if ((currentTime - startTime) > 10000){
    startTime= currentTime;
    
    // dont forget to blink
    blink();
          
    outterHumd = outterSens.readHumidity();
  	outterTemp = outterSens.readTemperature();    
    
    innerHumd= innerSens.readHumidity();
    innerTemp= innerSens.readTemperature();
    
    outterRh= outterHumd;
    outterT= outterTemp;
    
    innerRh= innerHumd;
    innerT= innerTemp;
    
    fanState= temperatureControl( outterT, innerT );
    fanSpeed= fanControl.getSpeed();
    
    // open a new sensor value line
    Serial1.print( "::" );
    // add header info
    Serial1.print( "X" );
    // add spacer
    Serial1.print(" ");

    // add sensor value
    Serial1.print( outterRh, DEC );
    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( outterT, DEC);    
    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( innerRh, DEC);    

    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( innerT, DEC);    

    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( fanState, DEC);    

    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( fanSpeed, DEC);    

    // finalize sensor line
    Serial1.println();              // wait
  }
}



