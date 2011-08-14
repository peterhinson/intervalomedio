#include "WProgram.h"
void setup();
void loop();
/*
 *  Intervalemedio.pde
 *  Peter Hinson / 2011
 *	mewp.net
 *	
 *  Intervalometer making use of a keypad and LCD menu system.
 *	Sort of an excuse to develop classes for these components.
 *
 */

#include <stdlib.h>
#include <wiring.h>
#include <hardwareserial.h>

#include "intervalomedio.h"
#include "memdebug.h"
#include "util.h"
#include "LCDMenu.h"
#include "Intervalometer.h"
#include "ADKeyboard.h"
#include "Event.h"


extern "C" void __cxa_pure_virtual() { for(;;); }


LCDMenu 		*menu;
ADKeyboard		*keypad;
Intervalometer	*timelapse;

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
	int n, i;
	Serial.begin(9600);
	
	menu 		= new LCDMenu;
	keypad	 	= new ADKeyboard(0);
	timelapse	= new Intervalometer(12, 13);
	
	menu->addSection(new LCDMenuSection);
	LCDMenuSection *menu_sec = menu->getCurrentSection();
	
	char start_stop[MAX_STATES][15]	= { "Start\0", "Stop\0" };
	char *btn_ptr[MAX_STATES];
	
	for (n = 0; n < 2; n++) {
		btn_ptr[n] = start_stop[n];
	}

	menu_sec->addParameter(new LCDMenuButton("Activity", kTimelapseControlEvent, btn_ptr, 2, 0, handleEvent));
	menu_sec->addParameter(new LCDMenuParameter("Interval (secs)", kIntervalEvent, 20.0f, 0.50f, 0.00, 172800.0, true, handleEvent));
	menu_sec->addParameter(new LCDMenuParameter("Exposure (msecs)", kExposureEvent, 250.0f, 25.0f, 25.0, 1200000.0, false, handleEvent));
	menu_sec->addParameter(new LCDMenuParameter("Backlight", kLCDBacklightEvent, 29.0f, 1.0f, 0.0, 29.0, false, handleEvent));
	menu_sec->addParameter(new LCDMenuButton("Memory Debug", kMemoryDebugNotice, btn_ptr, 2, 0, handleEvent));	
	if (memory_debug) showmem();
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
				menu->incCurrentParam(1);
				break;
			case 4:
				menu->incCurrentParam(-1);
				break;
			case 0:
				menu->clickCurrentParam();
				timelapse->triggerShutter();
				if (memory_debug) showmem();
				break;
			default:
				break;
		}
	//	menu->stayAwake();
	//	menu->setDirty(true);
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
			
		case kTimelapseControlEvent:
		Serial << "State: " << event.state << " . \n";
			if (event.state == kStartIntervalometer)
				timelapse->start();
			else
				timelapse->stop();
			break;
		
		case kMemoryDebugNotice:
			memory_debug = !memory_debug;
			break;
			
		default:
			break;
	}
	if (memory_debug) showmem();
}