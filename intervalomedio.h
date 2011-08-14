#ifndef Intervalomedio_h
#define Intervalomedio_h

#define DEBUG 					true
#define __cplusplus				true

#define MAX_STATES				8		// Most states/modes a menu item can have.

#define kStartIntervalometer	0
#define kStopIntervalometer		1

#define kIntervalEvent 			10
#define kExposureEvent 			11
#define kTimelapseControlEvent	15
#define kDelayEvent				12
#define kLCDBacklightEvent		20
#define kMemoryDebugNotice		50		

enum eDisplayType { TEXT, INT, FLOAT, MODE, BUTTON };

bool memory_debug = false;

#endif