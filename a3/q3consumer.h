#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include "q3buffer.h"
#include "MPRNG.h"

extern MPRNG mprng;
_Task Consumer {
	BoundedBuffer<int> *buffer;
	int delay, sentinel;
	int *sum;
	void main();
	public:
	Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum );
};

#endif
