#include "q3consumer.h"

Consumer::Consumer(BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum)
	: buffer(&buffer), delay(Delay), sentinel(Sentinel), sum(&sum) {};

void Consumer::main() {
	while (true) {
		yield(mprng(delay - 1));
		int item = buffer->remove();
		if (item == sentinel) {
			return;
		}
		*sum += item;
	} // while
}

