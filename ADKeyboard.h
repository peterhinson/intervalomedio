#include "WProgram.h"

class ADKeyboard {
	private:
		int in_pin;
	//	int adc_key_val[5];
		int NUM_KEYS;
		int adc_key_in;
		int last_adc;
		int key;
		int oldkey;
//		48 56 71 97 59
	public:
		ADKeyboard() {
			in_pin		= 0;
		 	//int adc_key_val[5]	= {50, 200, 400, 600, 800 };
			NUM_KEYS	= 5;
			key			= -1;
			oldkey		= -1;
			last_adc	= 0;
		}
		
		ADKeyboard(int pin) {
			in_pin		= pin;
		 	//adc_key_val[5]	= {50, 200, 400, 600, 800 };
			NUM_KEYS	= 5;
			key			= -1;
			oldkey		= -1;
			last_adc	= 0;
		}
		
		int readKeyboard()
		{
			adc_key_in = analogRead(in_pin);    // read the value from the sensor

			key = get_key(adc_key_in);  // convert into key press
			
			if (key != oldkey)   // if keypress is detected
			{

				delay(50);  // wait for debounce time
				adc_key_in = analogRead(in_pin);    // read the value from the sensor 
				key = get_key(adc_key_in);    // convert into key press
		//		Serial.print(key);
		//		Serial.print(" \n ");
				if (key != oldkey)    
				{   
					oldkey = key;

					if (key >=0) {
						return key;
					}
				}
		  	}
			return -1;	// No key change
		}

		// Convert ADC value to key number

		int get_key(unsigned int input)
		{
			int adc_key_val[5]	= {50, 200, 400, 600, 800 };
		    int k;
			for (k = 0; k < NUM_KEYS; k++)
			{
				if (input < adc_key_val[k]) return k;
			}

			if (k >= NUM_KEYS) k = -1;  // No valid key pressed
			return k;
		}
};
