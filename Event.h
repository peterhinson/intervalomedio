#ifndef Event_h
#define Event_h

struct Event 
{
	void				*object;
	unsigned long		time;
	int					source;
	int					type;
	float				value;
	int					state;
};

#endif