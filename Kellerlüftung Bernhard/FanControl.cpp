
#include <Arduino.h>
#include "FanControl.h"


FanControl::FanControl( char pin_, unsigned char def_speed ){

  fanState= FAN_INIT;
  defSpeed= def_speed;
  pwmPin= pin_;
  fanRampUpTimer= 0;
  newPwm= 0;  
  oldPwm= 0;
}


void FanControl::begin(){

  pinMode(pwmPin, OUTPUT);   
  setSpeed( defSpeed );
  
  fanState= FAN_OFF;
  switchOn();

}

void FanControl::process(){

  static unsigned long processTime= 0;
  
  // get current time
  unsigned long currentTime= millis();
  if ((currentTime - processTime) < CYCLE_TIME) {
	return;
  } else {
	processTime= currentTime;
  }
  

  switch ( fanState ){
	case FAN_OFF:
				break;

	case FAN_ON: 
				// write new value only when differnt
				if (newPwm != oldPwm){
					oldPwm= newPwm;
					setPWM( oldPwm );
				}
				break;
				
	case FAN_RAMP_UP:
				// wait for the fan reaching operting speed
				if (fanRampUpTimer>0){
				  fanRampUpTimer--;
				} else {
				  fanState= FAN_ON;
				  oldPwm=0;
				  Serial1.println("fan on");
				}
				break;
	
	case FAN_RAMP_DOWN:
				// switch off immediatly
				setPWM(255); 
				fanState= FAN_OFF;
  			    Serial1.println("fan off");
				break;
	
  }

}

char FanControl::getFanState(){
  return fanState;
}

unsigned char FanControl::getSpeed(){
  return (255-oldPwm);
}

char FanControl::setPWM( unsigned char pwm ){

	analogWrite(pwmPin, pwm); 
	Serial1.print("pwm change ");
	Serial1.println( pwm, DEC );

}

void FanControl::setSpeed( unsigned char speed ){
	
  if (speed < FAN_MIN_SPEED){
	switchOff();
	return;
  }
  if (speed > FAN_MAX_SPEED){
	speed= FAN_MAX_SPEED;
  }
  	
  // calc speed to pwm (invert)
  newPwm= 255- speed;
  
  // if fan is not enabled
  if (fanState == FAN_OFF){
	switchOn();
  }
  
  
}


void FanControl::switchOn(){

  if (fanState == FAN_OFF){
	fanRampUpTimer= FAN_RAMP_UP_TIMER;
	fanState= FAN_RAMP_UP;
	// set start-speed
	setPWM(128); 
  }

}

void FanControl::switchOff(){

  fanState= FAN_RAMP_DOWN;
}