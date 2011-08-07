#include "WProgram.h"

class ADKeyboard {
	private:
		int in_pin;
		int NUM_KEYS;
		int adc_key_in;
		int last_adc;
		int key;
		int oldkey;
		int repeat_delay;
		int repeat_rate;
		unsigned long previous_time;

	public:
		ADKeyboard(int pin = 0) 
		{
			in_pin					= pin;
			NUM_KEYS				= 5;
			key						= -1;
			oldkey					= -1;
			last_adc				= 0;
			repeat_delay			= 800;
			repeat_rate				= 150;
			previous_time			= 0;	
		}
		
		int readKeyboard()
		{
			adc_key_in	= analogRead(in_pin);   		// read the value from the sensor
			key 		= get_key(adc_key_in);  		// convert into key press
			
			if (key != oldkey) // if key change is detected
			{

				delay(50);								// wait for debounce time
				adc_key_in	= analogRead(in_pin);   	// read the value from the sensor 
				key 		= get_key(adc_key_in); 		// convert into key press
				
				if (key != oldkey)    
				{   
				//	Serial.print(key);
				//	Serial.print(" \n ");
					oldkey = key;
					
					if (key >=0) {
						previous_time = millis();
						return key;
					} else {
						previous_time = 0;
					}
				} 
		  	}	// Check if this key is being held down...
			else if (key >=0 && key < NUM_KEYS && previous_time > 0 && millis()-previous_time > repeat_delay) {
				// Held down, past the timeout... Repeat!
				previous_time	+= repeat_rate;
				return key;
			}
			return -1;	// No key change
		}

		// Convert ADC value to key number
		int get_key(unsigned int input)
		{	// See if the input values are associated with a key
			int adc_key_val[5]	= {50, 200, 400, 600, 800 };
		    int k;
			for (k = 0; k < NUM_KEYS; k++)
				if (input < adc_key_val[k]) return k;

			if (k >= NUM_KEYS) { previous_time = 0; k = -1; }  // No valid key pressed
			return k;
		}
};
