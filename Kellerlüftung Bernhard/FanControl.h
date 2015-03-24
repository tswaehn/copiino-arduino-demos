
#ifndef __FAN_H__
#define __FAN_H__

// config
#define CYCLE_TIME	100 // [ms]
#define FAN_MIN_SPEED	40  // 0..255
#define FAN_MAX_SPEED 250 // 0..255
#define FAN_RAMP_UP_TIMER 70 // [70] * cycle time 

// internal
#define FAN_INIT 			10
#define FAN_ON				1
#define FAN_OFF				2
#define FAN_RAMP_UP			3
#define FAN_RAMP_DOWN 		4


class FanControl {

public:
	FanControl( char pin_, unsigned char def_speed );
	
	// setup diverse things
	void begin();
	
	// call this process in the "main" loop
	void process();
	
	// set another speed
	void switchOn();
	void switchOff();
	void setSpeed( unsigned char speed );
		
	// return current state
	char getFanState();
	unsigned char getSpeed();
	
private:
	
	char fanState;
	char pwmPin;
	unsigned char oldPwm;
	unsigned char newPwm;
	unsigned char defSpeed;
	unsigned char fanRampUpTimer;
	
	char setPWM( unsigned char pwm );

};

#endif
