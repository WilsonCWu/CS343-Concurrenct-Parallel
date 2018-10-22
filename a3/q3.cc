#include <sstream>
#include <iostream>
#include <string>
#include <queue>
#include "MPRNG.h"

#include <cassert>
using namespace std;

MPRNG mprng;

template<typename T> class BoundedBuffer {
	uOwnerLock lock;
	uCondLock consLock, prodsLock;
	int volatile consSignal = 0, prodsSignal = 0;

	unsigned int size;
	queue<T>items;
	public:
	BoundedBuffer( const unsigned int size = 10 );
	void insert( T elem );
	T remove();
};

template<typename T>
BoundedBuffer<T>::BoundedBuffer(const unsigned int size): size(size) {}; 

template<typename T>
void BoundedBuffer<T>::insert(T elem) {
	lock.acquire();

#ifdef BUSY                            // busy waiting implementation
	while (items.size() == size) {
		prodsLock.wait(lock);
	}
#endif // BUSY
#ifdef NOBUSY                          // no busy waiting implementation
	if (prodsSignal > 0 || items.size() == size) {
		prodsLock.wait(lock);
		prodsSignal -= 1;
	}
#endif // NOBUSY

	assert(items.size() < size);
	items.push(elem);

#ifdef NOBUSY                          // no busy waiting implementation
	if (prodsSignal == 0 && !prodsLock.empty() && items.size() < size) {
		prodsSignal += 1;
		prodsLock.signal();
	}
	if (!consLock.empty()) {
		consSignal += 1;
	}
#endif // NOBUSY

	consLock.signal();
	lock.release();
}

template<typename T>
T BoundedBuffer<T>::remove() {
	lock.acquire();

#ifdef BUSY                            // busy waiting implementation
	while (items.size() == 0) {
		consLock.wait(lock);
	}
#endif // BUSY
#ifdef NOBUSY                          // no busy waiting implementation
	if (consSignal > 0 || items.size() == 0) {
		consLock.wait(lock);
		consSignal -= 1;
	}
#endif // NOBUSY

	assert(items.size() > 0);
	T elem = items.front();
	items.pop();

#ifdef NOBUSY                          // no busy waiting implementation
	if (consSignal == 0 && !consLock.empty() && items.size() > 0) {
		consSignal += 1;
		consLock.signal();
	}
	if (!prodsLock.empty()) {
		prodsSignal += 1;
	}
#endif // NOBUSY

	prodsLock.signal();
	lock.release();

	return elem;
}

_Task Producer {
	BoundedBuffer<int> *buffer;
	int produce, delay;
	void main();
	public:
	Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay );
};

Producer::Producer(BoundedBuffer<int> &buffer, const int Produce, const int Delay)
	:buffer(&buffer), produce(Produce), delay(Delay) {};

void Producer::main() {
	for (int i = 1; i <= produce; i++) {
		yield(mprng(delay - 1));
		buffer->insert(i);
	}
}



_Task Consumer {
	BoundedBuffer<int> *buffer;
	int delay, sentinel;
	int *sum;
	void main();
	public:
	Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum );
};

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
	}
}


bool convert( int &val, char *buffer ) {		// convert C string to integer
	std::stringstream ss( buffer );			// connect stream and buffer
	string temp;
	ss >> dec >> val;					// convert integer from buffer
	return ! ss.fail() &&				// conversion successful ?
		! ( ss >> temp );				// characters after conversion all blank ?
} // convert

void usage( char *argv[] ) {
	cerr << "Usage: " << argv[0]
		<< "  [ Cons [ Prods [ Produce [ BufferSize [ Delays ] ] ] ] ] " << endl;
	exit( EXIT_FAILURE );				// TERMINATE
} // usage

int main (int argc, char *argv[]) {
	int cons = 5, prods = 3, produce = 10, bufferSize = 10, delays;

	switch (argc) {
		case 6:
			if (!convert(delays, argv[5]) || delays <= 0) usage(argv);
		case 5:
			if (!convert(bufferSize, argv[4]) || bufferSize <= 0) usage(argv);
		case 4:
			if (!convert(produce, argv[3]) || produce <= 0) usage(argv);
		case 3:
			if (!convert(prods, argv[2]) || prods <= 0) usage(argv);
		case 2:
			if (!convert(cons, argv[1]) || cons <= 0) usage(argv);
		case 1:
			if (argc != 6) delays = cons + prods;
			break;
		default:
			usage(argv);
	} // switch

#ifdef __U_MULTI__
	uProcessor p[3] __attribute__(( unused )); // create 3 kernel thread for a total of 4
#endif // __U_MULTI__

	BoundedBuffer<int> buffer;
	int sentinel = -1;	// TODO: change this and type of buffer

	int total = 0;
	int sum[cons] = { 0 };
	Consumer *consumers[cons];
	Producer *producers[prods];

	for (int i = 0; i < prods; i++) {
		producers[i] = new Producer(buffer, produce, delays);
	}

	for (int i = 0; i < cons; i++) {
		consumers[i] = new Consumer(buffer, delays, sentinel, sum[i]);
	}

	for (int i = 0; i < prods; i++) {
		delete producers[i];
	}

	for (int i = 0; i < cons; i++) {
		buffer.insert(sentinel);
	}

	for (int i = 0; i < cons; i++) {
		delete consumers[i];
	}

	for (int i = 0; i < cons; i++) {
		total += sum[i];
	}

	cout << "total: " << total << endl;

} // main
