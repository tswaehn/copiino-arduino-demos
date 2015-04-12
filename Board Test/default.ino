/*
  Blink
  Turns on and off all onboard LEDs 
 
  This example code is in the public domain.

*/



// Pin 13 has an LED connected on most Arduino boards.
// give it a name: (from Left to Right)
int led13 = 13; // default Arduino LED (LED is on if Pin13 is High)
int led21 = 21; // additional CoPiino LED (LED on if Pin21 is Low)
int led20 = 20; // additional CoPiino LED (LED on if Pin20 is Low)


// 
unsigned int detectedFrequency=0; // Hz
unsigned int targetFrequency= 10; // Hz


unsigned int testCycleOk=0;
unsigned int testCycleFail=0;
char outChannel=0;
char inChannel=1;


// interrupt service routine 
ISR(TIMER1_OVF_vect)          
{
	#define TIMER_FREQ 7812.5

	
  // out vars
	static unsigned char freqCounter;
  static unsigned int timeCount=0;
  static char outValue=0;
  // in vars
  static unsigned int inTimeCount=0;
  static char lastInValue=0;
  char inValue=0;
  
  // generate the output frequency
  if (timeCount <= 0){
    timeCount= (float)(TIMER_FREQ/2) / (float)(2*targetFrequency)+0.5; 
	  
    // toggle output
    outValue= 1-outValue;
    digitalWrite( outChannel, outValue);
  	  
  } else {
    timeCount--;
  }    
  
  // detect the input frequency
  inValue= digitalRead( inChannel );
  inTimeCount++;
  if (inValue != lastInValue){
    detectedFrequency= (float)(TIMER_FREQ/2)/ ((float)(inTimeCount*2))+0.5;
    inTimeCount= 0;
    testCycleOk++;
    testCycleFail=0;
    // toggle LED at detected frequency
    digitalWrite( led21, inValue );
  }
  lastInValue= inValue;

  if (inTimeCount > TIMER_FREQ){
   inTimeCount= 0; 
   detectedFrequency= 0;
   testCycleFail++;
   testCycleOk= 0;
   // LED off
   digitalWrite( led21, 1 );
  }
  
}

void setChannelTestPair( char in, char out ){
  
  cli();
  // disable current channels
  pinMode(inChannel, INPUT);
  pinMode(outChannel, INPUT);
  
  // set channels
  inChannel= in;
  outChannel= out;
  
  // enable new channels
  pinMode(inChannel, INPUT);
  pinMode(outChannel, OUTPUT);
  targetFrequency= 10;
	sei();
  
}


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
   
    
    return;
  }

  // freq
  if (strcmp("freq", array[0] ) == 0){
    fValue0 = strtod( array[1], 0 );
   	
    // set new target frequency
    cli();
    targetFrequency= fValue0;    
    sei();
    
    return;
  }  
  
  // chan
  if (strcmp("chan", array[0] ) == 0){
    fValue0 = strtod( array[1], 0 );
    fValue1 = strtod( array[2], 0 );
   	
    setChannelTestPair(fValue0, fValue1);
    
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

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.

  pinMode(led20, OUTPUT);     
  pinMode(led21, OUTPUT);

  
  pinMode(0, OUTPUT);
  pinMode(1, INPUT);
  
	// setup timer

  // opens serial port 1 to Raspberry, sets data rate to 115200 bps
  // serial port 0 is as usual the Arduino port
  Serial1.begin(115200);
  Serial1.println("- start program - ");  
  
  
	//clock_prescale_set(clock_div_1);
  CLKPR = 0x80 | 0x0 ;
  // timer to 7812.5Hz
  TCCR1A = TCCR1A & 0b11111100 | 0x01;
  TCCR1B = TCCR1B & 0b11100000 | 0x02 | 0x00;

  // enable timer1 interrupt
  TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
  // enable global interrupts
  sei();                                                   // ensures that interrupts are globally enabled
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  static char b=0;
  unsigned int detectFreq, targetFreq;
  unsigned int cycleOk, cycleFail;

  
  static unsigned long startTime=0;
  unsigned long currentTime;

  // check for incomming commands
	receiveCommand();
  
  
  // run every 500ms something
  currentTime= millis();
  if ((currentTime - startTime) > 500){
    startTime= currentTime;
    
    // dont forget to blink
	  b= 1-b;
  	digitalWrite(led20, b);   // turn the LED 20 off 
    
    //
    cli();
    detectFreq= detectedFrequency;
    targetFreq= targetFrequency;
    cycleOk= testCycleOk;
    cycleFail= testCycleFail;    
    sei();
    
    // open a new sensor value line
    Serial1.print( "::" );
    // add header info
    Serial1.print( "X" );
    // add spacer
    Serial1.print(" ");

    // add sensor value
    Serial1.print( targetFreq, DEC );
    // add spacer
    Serial1.print(" ");
    // add sensor value
    Serial1.print( detectFreq, DEC );
    // add spacer
    Serial1.print(" ");
    // add sensor value
    Serial1.print( outChannel, DEC );
    // add spacer
    Serial1.print(" ");
    // add sensor value
    Serial1.print( inChannel, DEC );
    // add spacer
    Serial1.print(" ");
    // add sensor value
    Serial1.print( cycleOk, DEC );
    // add spacer
    Serial1.print(" ");
    // add sensor value
    Serial1.print( cycleFail, DEC );
   
    // finalize sensor line
    Serial1.println();
    
  }

}



