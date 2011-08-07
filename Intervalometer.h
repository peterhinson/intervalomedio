/*
 *  Intervalometer
 *  Peter Hinson / 2011
 *	mewp.net
 *
 *  Intervalometer logic, triggering a board of reed switches connected to the camera.
 *	Designed for Canon SLRs, but should be usable for whateves.
 *
 *	Adapted from the excellent Intervaluino by "Lord Yo":
 *	http://www.sporez.com/honeyjar/?p=347
 *	
 */

#ifndef Intervalometer_h
#define Intervalometer_h

#include "WProgram.h"


typedef unsigned long ulong;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * Intervalometer Prototype
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class Intervalometer 
{
	public:
		int lapse_time;			// Delay between exposures, in seconds
		int exposure_time;		// Exposure. 1000 = 1 sec
		

		int shutter_on;			// time to press shutter, set between 100 and 300
		int shutter_wait;		// Initial time to wait to begin sequence
		int wakeup;			  	// Time to activate wakeup (focus)
		int wake_wait;		 	// Time between wake and shutter

		int frame_limit;		// Number of frames at which to stop
		int frame_count;
		
		bool focus;
		bool active;

		unsigned long previous_time;	// Previous shutter click (from start of the exposure)
		
		Intervalometer();
		Intervalometer(int in_focus_pin, int in_shutter_pin);

		void loop();
		
		void triggerShutter();
		void wakeAndFocus();
		void start();
		
		void stop();
		
		void setInterval(float seconds);
		
	private:
		int focus_pin;			// The focus pin is also used to wake up the camera
		int shutter_pin;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * Intervalometer Methods
 * *  ---------------------------------------------------------
 * *	These will be moved to a cpp when I figure out how to
 * * 	compile that from TextMate.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Intervalometer::Intervalometer() 
{
	lapse_time		= 1000;          

	focus_pin		= 9;        
	shutter_pin		= 7;

	shutter_on		= 200;     
	shutter_wait	= 5000;	
	wakeup			= 300;	
	wake_wait		= 200;
	
	focus			= false;
	active			= true;
	
	frame_count		= 0;
	
 	pinMode(shutter_pin, OUTPUT);
	pinMode(focus_pin, OUTPUT);
}

Intervalometer::Intervalometer(int in_focus_pin = 9, int in_shutter_pin = 7) 
{
	lapse_time		= 1000;          

	focus_pin		= in_focus_pin;        
	shutter_pin		= in_shutter_pin;

	shutter_on		= 200;     
	shutter_wait	= 5000;	
	wakeup			= 300;	
	wake_wait		= 200;  
	
	focus			= false;
	active			= true;
	
	previous_time	= 0;
	frame_count		= 0;
	
 	pinMode(shutter_pin, OUTPUT);
	pinMode(focus_pin, OUTPUT);
}

void Intervalometer::loop() 
{
	if (active && millis() - previous_time > lapse_time) {
		// Could take into account wakeup/focus time and substract from lapse_time above?
		wakeAndFocus();
		triggerShutter();
		
		if (frame_limit != -1 && frame_count >= frame_limit)
	   		stop();
	}
}

void Intervalometer::triggerShutter() 
{
	previous_time = millis();			// Record the time that we start the exposure
	
    digitalWrite(shutter_pin, HIGH);
    delay(shutter_on);					// Should fuck with this, unsure what the proper value is.
    digitalWrite(shutter_pin, LOW);
	
    frame_count++;
}

void Intervalometer::wakeAndFocus() 
{
	digitalWrite(focus_pin, HIGH);        // Wake the camera up/focus
    delay(wakeup);                        // Wait for it...
    digitalWrite(focus_pin, LOW);
	
    delay(wake_wait);
}

void Intervalometer::start() 
{
	previous_time	= 0;
	active			= true;
	frame_count		= 0;
}

void Intervalometer::stop() 
{
	active = false;
}

void Intervalometer::setInterval(float seconds) 
{
	lapse_time = (int)(seconds*1000.0f);
}


#endif