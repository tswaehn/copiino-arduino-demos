/*
 	RS232 Cmd Demo
  
  This program shows how to receive commands from the Raspberry Pi.
  There is one command "led" that enables or disables the onboard
  LEDs.
  
  ex. "led 20 1" means enable LED 20
  		"led 21 0" means disable LED 21
 
  This example code is in the public domain.

	by
  sven@copiino.cc
  01.dec.2014

 */
 
// define LED20
int led = 13;
char led20= 20;
char led21= 21;

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
  if (strcmp("led", array[0] ) == 0){
    fValue0 = strtod( array[1], 0 );
    fValue1 = strtod( array[2], 0 );    
		
    value0= (int) fValue0;
    value1= (int) fValue1;
    
    switch( value0 ){
      case 20: value1?digitalWrite(led20, LOW):digitalWrite(led20, HIGH);
      				 break;

      case 21: value1?digitalWrite(led21, LOW):digitalWrite(led21, HIGH);
      				 break;
    }
    
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
    // output something
    Serial1.println("LED 13 on");
    
		// turn the LED on (HIGH is the voltage level)
	  digitalWrite(led, HIGH);   
    
  } else {
    // output something
    Serial1.println("LED 13 off");
		
    // turn the LED off by making the voltage LOW    
    digitalWrite(led, LOW);    
  }
}


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT); 
  pinMode(led20, OUTPUT); 
  pinMode(led21, OUTPUT); 
  
  digitalWrite(led20, HIGH);    // turn the LED off by making the voltage HIGH
  digitalWrite(led21, HIGH);    // turn the LED off by making the voltage HIGH
  
  // opens serial port 1 to Raspberry, sets data rate to 115200 bps
  // serial port 0 is as usual the Arduino port
  Serial1.begin(115200);
  Serial1.println("- start program - ");  
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  static unsigned long startTime=0;
  unsigned long currentTime;

  // check for incomming commands
	receiveCommand();
  
  
  // run every 100ms something
  currentTime= millis();
  if ((currentTime - startTime) > 500){
    startTime= currentTime;
    
    // dont forget to blink
    blink();
    
  }
  
}



