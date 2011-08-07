/*
 *  LCD Menu System
 *  Peter Hinson / 2011
 *
 *  Designed for Sparkfun Serial Enabled LCD screens.
 *  Provides a basic, navigatable menu system.
 *
 */
#include <stdlib.h>
/*
__extension__ typedef int __guard __attribute__((mode (__DI__)));

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

*/

void * operator new(size_t size);
void operator delete(void * ptr);

void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}

typedef unsigned long ulong;





class LCDMenuParameter {
	private:
		char	*name;
		float	value;
		float	inc;
		bool	display_float;

	public:
		LCDMenuParameter() { }
		
		LCDMenuParameter(char in_name[], float in_value, float in_inc, bool in_display_float) {
			name			= in_name;
			value			= in_value;
			inc 			= in_inc;
			display_float	= in_display_float;
		}
	
		
		void init(char in_name[], float in_value, float in_inc, bool in_display_float) {
			name			= in_name;
			value			= in_value;
			inc 			= in_inc;
			display_float	= in_display_float;
		}
		
		void onSave(void(*function)(int)) {
		//	user_onSave = function;
		}
		
		float getValue() {
			return value;
		}
		
		char* getName() {
			return name;
		}
		
		void setValue(float new_value) {
			value = new_value;
		}
		
		void incValue(float steps) {
			value += (inc*steps);
		}
};

#define max_params 8
class LCDMenuSection {
	private:
		//LCDMenuItem[]		submenus;
		LCDMenuParameter	*params[8];
		int					num_params;
		int					index;				// Currently selected param/submenu
		
	public:
		LCDMenuSection() {
			num_params = 0;
			index = 0;
		}
		
		LCDMenuParameter* getCurrentParameter() {
			if (index >= 0 && index < num_params)
				return params[index];
			else
				return new LCDMenuParameter();
		}
		
		void addParameter(LCDMenuParameter *new_param) {
			if (num_params < 8) {
				params[num_params++] = new_param;
			}	// TODO: Should fail gracefully... or create a new array!
		}
		
		
		void nextItem() { if (index < num_params-1) index++; else index = 0; }
		void prevItem() { if (index > 0) index--; else index = num_params-1; }
};

class LCDMenu {
	private:
		int				index;
		bool			dirty;
		LCDMenuSection	*root;
		LCDMenuSection	*cur_section;
		
	public:
		LCDMenu() {
			dirty = true;
			clearLCD();
			backlightOn();
			cur_section = NULL;
		};
		
		
		void printMenu() {
			if (dirty) {
				dirty = false;
				clearLCD();
				
				LCDMenuParameter *cur_param = cur_section->getCurrentParameter();
				selectLineOne();
				Serial.print(cur_param->getName());
				selectLineTwo();
				Serial.print(cur_param->getValue());		
			}
		}
		
		void nextItem() {
			setDirty(true);
			cur_section->nextItem();
		}
		
		void prevItem() {
			setDirty(true);
			cur_section->prevItem();
		}
		
		void incCurrentParam(float inc) {
			setDirty(true);
			cur_section->getCurrentParameter()->incValue(inc);
		}
		
		void addSection(LCDMenuSection *section) {
			root		= section;
			cur_section	= section;
		}
		
		LCDMenuSection * getCurrentSection() {
			return cur_section;
		}
		
		void setDirty(bool is_dirty) {
			dirty = is_dirty;
		}
		
		bool isDirty() {
			return dirty;
		}
		
		void selectLineOne(){  //puts the cursor at line 0 char 0.
		   Serial.print(0xFE, BYTE);   //command flag
		   Serial.print(128, BYTE);    //position
		   delay(10);
		}

		void selectLineTwo(){  //puts the cursor at line 0 char 0.
		   Serial.print(0xFE, BYTE);   //command flag
		   Serial.print(192, BYTE);    //position
		   delay(10);
		}
		void goTo(int position) { //position = line 1: 0-15, line 2: 16-31, 31+ defaults back to 0
		if (position<16){ Serial.print(0xFE, BYTE);   //command flag
		              Serial.print((position+128), BYTE);    //position
		}else if (position<32){Serial.print(0xFE, BYTE);   //command flag
		              Serial.print((position+48+128), BYTE);    //position 
		} else { goTo(0); }
		   delay(10);
		}

		void clearLCD(){
		   Serial.print(0xFE, BYTE);   //command flag
		   Serial.print(0x01, BYTE);   //clear command.
		   delay(10);
		}
		void backlightOn(){  //turns on the backlight
		    Serial.print(0x7C, BYTE);   //command flag for backlight stuff
		    Serial.print(157, BYTE);    //light level.
		   delay(10);
		}
		void backlightOff(){  //turns off the backlight
		    Serial.print(0x7C, BYTE);   //command flag for backlight stuff
		    Serial.print(128, BYTE);     //light level for off.
		   delay(10);
		}
		void serCommand(){   //a general function to call the command flag for issuing all other commands   
		  Serial.print(0xFE, BYTE);
		}
};

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

class Intervalometer {
	public:
		int lapse_time;			// Delay between exposures, in seconds
		int exposure_time;		// Exposure. 1000 = 1 sec
		int focus_pin;			// The focus pin is also used to wake up the camera
		int shutter_pin;

		int shutter_on;			// time to press shutter, set between 100 and 300
		int shutter_wait;		// Initial time to wait to begin sequence
		int wakeup;			  	// Time to activate wakeup (focus)
		int wake_wait;		 	// Time between wake and shutter

		int frame_limit;		// Number of frames at which to stop
		int frame_count;
		
		bool focus;
		bool active;

		ulong previous_time;	// Previous shutter click (from start of the exposure)
		
		Intervalometer(in_focus_pin = 9, in_shutter_pin = 7, ) {
			lapse_time		= 5;          

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

		void loop() {
			if (active && millis() - previous_time > lapse_time) {
				// Could take into account wakeup/focus time and substract from lapse_time above?
			    digitalWrite(focus_pin, HIGH);        // Wake the camera up/focus
			    delay(wakeup);                        // Wait for it...
			    digitalWrite(focus_pin, LOW);

			    delay(wake_wait);
				
				previous_time = millis();			// Record the time that we start the exposure
			    digitalWrite(shutter_pin, HIGH);
			    delay(shutter_on);					// Should fuck with this, unsure what the proper value is.
			    digitalWrite(shutter_pin, LOW);
				
				
//			    delay(lapse_time*1000);
			    frame_count++;
				if (frame_limit != -1 && frame_count >= frame_limit)
			   		stop();
			}
		}
		
		void start() {
			previous_time	= 0;
			active			= true;
			frame_count		= 0;
		}
		
		void stop() {
			active = false;
		}
		
		void setLapseTime(float seconds) {
			lapse_time = (int)(seconds*1000.0f);
		}
};


LCDMenu 	*menu	= NULL;
ADKeyboard	keypad;

void setup()
{
	
	Serial.begin(9600);
	
	menu 	= new LCDMenu;
	keypad 	= ADKeyboard(0);
	
	menu->addSection(new LCDMenuSection);
	LCDMenuSection *menu_sec = menu->getCurrentSection();
	
	menu_sec->addParameter(new LCDMenuParameter("Item Five", 1.0f, 1.0f, true));
	menu_sec->addParameter(new LCDMenuParameter("Item Three", 1.0f, 1.0f, true));
	menu_sec->addParameter(new LCDMenuParameter("Item Four", 1.0f, 1.0f, true));
	//Serial.print("test..");
}

void loop()
{  
	int key = keypad.readKeyboard();
	if (key != -1) {
		switch (key) {
			case 2:
				menu->nextItem();
				break;
			case 3:
				menu->prevItem();
				break;
			case 1:
				menu->incCurrentParam(1.0f);
				break;
			case 4:
				menu->incCurrentParam(-1.0f);
				break;
			default:
				break;
		}
	}
	menu->printMenu();
	
	delay(100);
}

