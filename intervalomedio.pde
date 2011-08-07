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
#include "Event.h"

LCDMenu 		*menu;
ADKeyboard		*keypad;
Intervalometer	*timelapse;

#define kIntervalEvent 	10
#define kExposureEvent 	11
#define kDelayEvent		12

void handleEvent(Event);

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
	menu_sec->addParameter(new LCDMenuParameter("Interval (secs)", kIntervalEvent, 5.0f, 1.0f, true, handleEvent));
	menu_sec->addParameter(new LCDMenuParameter("Exposure (msecs)", kExposureEvent, 250.0f, 50.0f, true, handleEvent));
	menu_sec->addParameter(new LCDMenuParameter("Item Four", kDelayEvent, 1.0f, 1.0f, true, handleEvent));
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

void handleEvent(Event event) {
	switch (event.source) {
		case kIntervalEvent:
			timelapse->setInterval(event.value);
			break;
		default:
			break;
	}
}

void setExpos(float) {
	
}
