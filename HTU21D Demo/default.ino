/*
  Blink
  Turns on and off all LEDs 
 
  This example code is in the public domain.

*/

#include "Wire.h"
#include "HTU21D.h"
  
HTU21D myHumidity;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
// int led0 = 13;
int led1 = 21; // additional CoPiino LED
int led2 = 20; // additional CoPiino LED


// just blink some LED
void blink(){
  static char i=0;

  // invert i
  i= 1-i;
  
  // switch LED depending on i
  if (i){
	  digitalWrite(led2, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(led2, LOW);    // turn the LED off by making the voltage LOW
  }
}


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  //pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);     
  pinMode(led2, OUTPUT);  

  // opens serial port 1 to Raspberry, sets data rate to 115200 bps
  // serial port 0 is as usual the Arduino port
  Serial1.begin(115200);
  Serial1.println("- start program - ");  
  
  Wire.begin();
  
	myHumidity.begin();  
}

// the loop routine runs over and over again forever:
void loop() {

  static unsigned long startTime=0;
  unsigned long currentTime;
  static char x=0;
  static char y=0;
  static char i=0;

  float humd;
  float temp;
  
  // print every 100ms something
  currentTime= millis();
  if ((currentTime - startTime) > 100){
    startTime= currentTime;
    
    // dont forget to blink
    blink();
          
    humd = myHumidity.readHumidity();
  	temp = myHumidity.readTemperature();    
    
    x= humd;
    y= temp;
    
    // open a new sensor value line
    Serial1.print( "::" );
    // add header info
    Serial1.print( "X" );
    // add spacer
    Serial1.print(" ");

    // add sensor value
    Serial1.print( x, DEC );
    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( y, DEC);    
    // add spacer
    Serial1.print(" ");
    
    // add another sensor value
		Serial1.print( i, DEC);    

    // finalize sensor line
    Serial1.println();              // wait
  }
}



