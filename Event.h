#ifndef Event_h
#define Event_h

#include "WProgram.h"

struct Event {
//	LCDMenuParameter	*param;
	unsigned long		time;
	int					source;
	int					type;
	float				value;
};

#endif