#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "q3buffer.h"
#include "MPRNG.h"

extern MPRNG mprng;

_Task Producer {
	BoundedBuffer<int> *buffer;
	int produce, delay;
	void main();
	public:
	Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay );
};

#endif
