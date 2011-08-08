/*
 *  Intervalemedio.pde
 *  Peter Hinson / 2011
 *	mewp.net
 *	
 *  Intervalometer making use of a keypad and LCD menu system.
 *	Sort of an excuse to develop classes for these components.
 *
 */

#define DEBUG 					true
#define __cplusplus				true

#include <stdlib.h>
 
#include "memdebug.h"
#include "util.h"
#include "LCDMenu.h"
#include "Intervalometer.h"
#include "ADKeyboard.h"
#include "Event.h"

#include <wiring.h>
#include <hardwareserial.h>

extern "C" void __cxa_pure_virtual() { for(;;); }


LCDMenu 		*menu;
ADKeyboard		*keypad;
Intervalometer	*timelapse;

#define kIntervalEvent 			10
#define kExposureEvent 			11
#define kDelayEvent				12
#define kLCDBacklightEvent		20

enum eDisplayType { TEXT, INT, FLOAT, MODE, BUTTON };

/*
class ParameterFormatter {
	eDisplayType			displayType;
	
	<template>
	format
}
*/


void showmem()
{
  char buffer[100];
 
  sprintf(buffer,"\n\n%04u %04u %04u : used/free/large",
      getMemoryUsed(),
      getFreeMemory(),
      getLargestAvailableMemoryBlock()
    );
 
  Serial.println(buffer);
}





void handleEvent(Event);

void setup()
{
	
	Serial.begin(9600);
	showmem();
	menu 		= new LCDMenu;
	keypad	 	= new ADKeyboard(0);
	timelapse	= new Intervalometer(12, 13);
	
	menu->addSection(new LCDMenuSection);
	LCDMenuSection *menu_sec = menu->getCurrentSection();
	String *toggle	= new String("Ok, sort of progress");
//	char *toggle	= {test, test2};

	menu_sec->addParameter(new LCDMenuButton("Activity", kDelayEvent, *toggle, 6, 1, handleEvent));	
	menu_sec->addParameter(new LCDMenuParameter("Interval (secs)", kIntervalEvent, 60.0f, 0.50f, 0.00, 172800.0, true, handleEvent));
//	menu_sec->addParameter(new LCDMenuParameter("Exposure (msecs)", kExposureEvent, 250.0f, 25.0f, 25.0, 1200000.0, false, handleEvent));
	menu_sec->addParameter(new LCDMenuParameter("Backlight", kLCDBacklightEvent, 29.0f, 1.0f, 0.0, 29.0, false, handleEvent));
	showmem();
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
			case 0:
				timelapse->triggerShutter();
				break;
			default:
				break;
		}
		menu->stayAwake();
		menu->setDirty(true);
		showmem();
	}
	timelapse->loop();
	menu->printMenu();
//	delay(30);
}

void handleEvent(Event event) {
	switch (event.source) {
		case kIntervalEvent:
			timelapse->setInterval(event.value);
			
			break;
		case kLCDBacklightEvent:
			menu->backlightBrightness((int)(event.value));
			break;
			
		default:
			break;
	}
}

void setExpos(float) {
	
}
