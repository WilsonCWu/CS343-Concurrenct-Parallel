#include "q2potato.h"
#include <iostream>

using std::cout;
using std::endl;

Potato::Potato(unsigned int maxTicks): maxTicks(maxTicks) {
	reset();
}

void Potato::reset(unsigned int maxTicks) {
	tick = prng(1, maxTicks);
	cout << "  POTATO goes off after "
		<< tick
		<< (tick == 1 ? " toss" : " tosses")
		<< endl;
}

void Potato::countdown() {
	tick -= 1;
	if (tick == 0) throw Explode();
}

