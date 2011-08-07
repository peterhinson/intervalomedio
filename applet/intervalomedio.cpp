#include "WProgram.h"
void setup();
void loop();
/*
 *  Intervalemedio
 *  Peter Hinson / 2011
 *	mewp.net
 *	
 *  Intervalometer making use of a keypad and LCD menu system.
 *	Sort of an excuse to develop classes for these components.
 *
 */

#include <stdlib.h>
#include "util.h"

#include "AikoCallback.h"
#include "Intervalometer.h"
#include "LCDMenu.h"
#include "ADKeyboard.h"

LCDMenu 		*menu;
ADKeyboard		*keypad;
Intervalometer	*timelapse;

void eventHandler(char*, float) {
	
}

void setup()
{
	Serial.begin(9600);
	
	menu 		= new LCDMenu;
	keypad	 	= new ADKeyboard(0);
	timelapse	= new Intervalometer(12, 13);
	
	menu->addSection(new LCDMenuSection);
	LCDMenuSection *menu_sec = menu->getCurrentSection();
//	SetValueCallback cb = *(timelapse->setInterval);
//	Callback cb = methodCallback(*timelapse, &Intervalometer::setInterval );
	menu_sec->addParameter(new LCDMenuParameter("Interval (seconds)", 5.0f, 1.0f, true));
	menu_sec->addParameter(new LCDMenuParameter("Exposure", 1.0f, 1.0f, true));
	menu_sec->addParameter(new LCDMenuParameter("Item Four", 1.0f, 1.0f, true));
	Serial.print("test..");
}

void loop()
{  
	int key = keypad->readKeyboard();
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

