#include "q3producer.h"

Producer::Producer(BoundedBuffer<int> &buffer, const int Produce, const int Delay)
	:buffer(&buffer), produce(Produce), delay(Delay) {};

void Producer::main() {
	for (int i = 1; i <= produce; i++) {
		yield(mprng(delay - 1));
		buffer->insert(i);
	} // for
}

