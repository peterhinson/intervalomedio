/*
 *  LCD Menu System
 *  Peter Hinson / 2011
 *	mewp.net
 *
 *  Provides a basic, navigatable menu system.
 *	Works as is with Sparkfun serial enabled LCD screens.
 *	All model specific code is contained within the LCDMenu class.
 */

#ifndef LCDMenu_h
#define LCDMenu_h

#include <stdlib.h>
#include "WProgram.h"
#include "AikoCallback.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * LCDMenuParameter
 * *  ---------------------------------------------------------
 * *	Single menu items. Stores values, display names, etc. 
 * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		
class LCDMenuParameter {
	private:
		char					*_name;
		float					_value;
		float					_inc;
		bool					_display_float;
		
		Callback			_setValueCallback;
		
	public:
		LCDMenuParameter() { }
		
		void doNothing(float) {}
		
		LCDMenuParameter(char in_name[], float in_value, float in_inc, bool in_display_float) 
		{
			_name				= in_name;
			_value				= in_value;
			_inc 				= in_inc;
			_display_float		= in_display_float;
		//	_setValueCallback	= setValueCallback;
		}
//		typedef void (*SetValueCallback)(const float);
//		typedef std::tr1::function<void (float)> setValueCallback;			// Called for each value change
//		typedef std::tr1::function<void (float)> saveValueCallback;			// Called when no changes have been made for a timeout
		
		float getValue()
		{
			return _value;
		}
		
		char* getName()
		{
			return _name;
		}
		
		void setValue(float new_value)
		{
			if (_value != new_value) {
				_value = new_value;
			//	if (_setValueCallback != NULL) {}
			//		_setValueCallback(new_value);		// If a callback is set for this value, call it.
			}
		}
		
		void incValue(float steps)
		{
			setValue(_value + (_inc*steps));
		}
		
		void registerSetValueCallback(Callback newCallback)
		{
			this->_setValueCallback = newCallback;
		}
		
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * LCDMenuSection
 * *  ---------------------------------------------------------
 * *	A section or submenu. Currently can only contain
 * *	parameters as items, but could be expanded for nesting.
 * *	
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class LCDMenuSection {
	private:
		//LCDMenuItem[]		submenus;
		LCDMenuParameter	*params[8];
		int					num_params;
		int					index;				// Currently selected param/submenu
		
	public:
		LCDMenuSection()
		{
			num_params = 0;
			index = 0;
		}
		
		LCDMenuParameter* getCurrentParameter()
		{
			if (index >= 0 && index < num_params)
				return params[index];
			else
				return new LCDMenuParameter();
		}
		
		void addParameter(LCDMenuParameter *new_param)
		{
			if (num_params < 8) {
				params[num_params++] = new_param;
			}	// TODO: Should fail gracefully... or create a new array!
		}
		
		
		void nextItem() { if (index < num_params-1) index++; else index = 0; }
		void prevItem() { if (index > 0) index--; else index = num_params-1; }
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * LCDMenu
 * *  ---------------------------------------------------------
 * *	Contains the display logic on the menu system, interfaces
 * *	with the hardware. Somewhat model specific.
 * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class LCDMenu {
	private:
		int				index;
		bool			dirty;
		LCDMenuSection	*root;
		LCDMenuSection	*cur_section;
		
	public:
		LCDMenu()
		{
			dirty = true;
			clearLCD();
			backlightOn();
			cur_section = NULL;
		};
		
		void printMenu()
		{
			if (dirty) {					// If marked for redraw...
				dirty = false;
				clearLCD();
				
				LCDMenuParameter *cur_param = cur_section->getCurrentParameter();
				selectLineOne();
				Serial.print(cur_param->getName());
				selectLineTwo();
				Serial.print(cur_param->getValue());		
			}
		}
		
		void nextItem() 
		{
			setDirty(true);
			cur_section->nextItem();
		}
		
		void prevItem() 
		{
			setDirty(true);
			cur_section->prevItem();
		}
		
		void incCurrentParam(float inc) 
		{
			setDirty(true);
			cur_section->getCurrentParameter()->incValue(inc);
		}
		
		void addSection(LCDMenuSection *section) 
		{
			// Add a submenu or group of parameters... LCDMenu only works with one section currently.
			root		= section;		// Temp. Should only be for the first item.
			cur_section	= section;		// Set this to the active section 
		}
		
		LCDMenuSection * getCurrentSection() 
		{
			return cur_section;
		}
		
		void setDirty(bool is_dirty) 
		{
			dirty = is_dirty;			// Mark LCD for refresh
		}
		
		bool isDirty() 
		{
			return dirty;
		}
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
			Danger...
			Model specific code follows. Override?
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		void selectLineOne()
		{	// Puts the cursor at line 0 char 0.
			Serial.print(0xFE, BYTE);   //command flag
			Serial.print(128, BYTE);    //position
			delay(10);
		}

		void selectLineTwo()
		{	// Puts the cursor at line 0 char 0.
			Serial.print(0xFE, BYTE);   //command flag
			Serial.print(192, BYTE);    //position
			delay(10);
		}
		
		void goTo(int position)
		{	// Position = line 1: 0-15, line 2: 16-31, 31+ defaults back to 0
			if (position<16) {
				Serial.print(0xFE, BYTE);   				//command flag
				Serial.print((position+128), BYTE);    		//position
			} else if (position<32) {
				Serial.print(0xFE, BYTE);  					//command flag
				Serial.print((position+48+128), BYTE);		//position 
			} else { goTo(0); }
			delay(10);
		}

		void clearLCD()
		{	// Clears the LCD. 
			LCDCommand();   			// command flag
	 		Serial.print(0x01, BYTE);   // clear command.
			delay(10);
		}
		
		void backlightOn()
		{	// Turns on the backlight
		    Serial.print(0x7C, BYTE);   // command flag for backlight stuff
		    Serial.print(157, BYTE);    // light level.
			delay(10);
		}
		void backlightOff()
		{	// Turns off the backlight
			Serial.print(0x7C, BYTE);   // command flag for backlight stuff
			Serial.print(128, BYTE);	// light level for off.
			delay(10);
		}
		void LCDCommand()
		{   // A general function to call the command flag for issuing all other commands   
			Serial.print(0xFE, BYTE);
		}
};

#endif
