#ifndef Event_h
#define Event_h

#include "WProgram.h"

struct Event 
{
	void				*object;
	unsigned long		time;
	int					source;
	int					type;
	float				value;
};

#endif