#include <sstream>
#include <iostream>
#include <string>
#include "q3consumer.h"
#include "q3producer.h"
#include "MPRNG.h"
MPRNG mprng;

using namespace std;
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

	BoundedBuffer<TYPE> buffer;

	int total = 0;
	int sum[cons] = { 0 };
	Consumer *consumers[cons];
	Producer *producers[prods];

	for (int i = 0; i < prods; i++) {	// create producers
		producers[i] = new Producer(buffer, produce, delays);
	} // for

	for (int i = 0; i < cons; i++) {	// create consumers
		consumers[i] = new Consumer(buffer, delays, SENTINEL, sum[i]);
	} // for

	for (int i = 0; i < prods; i++) {	// wait for producers done
		delete producers[i];
	} // for

	for (int i = 0; i < cons; i++) {	// insert sentinels
		buffer.insert(SENTINEL);
	} // for

	for (int i = 0; i < cons; i++) {	// wait for consumers done
		delete consumers[i];
	} // for

	for (int i = 0; i < cons; i++) {	// sum up total
		total += sum[i];
	} // for

	cout << "total: " << total << endl;

} // main
