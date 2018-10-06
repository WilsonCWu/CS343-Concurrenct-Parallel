#ifndef __POTATO_H__
#define __POTATO_H__

#include "PRNG.h"

extern PRNG prng;
class Potato {
	unsigned int tick;
	unsigned int maxTicks;
	public:
	_Event Explode {};
	Potato( unsigned int maxTicks = 10 );
	void reset( unsigned int maxTicks = 10 );
	void countdown();
};

#endif
