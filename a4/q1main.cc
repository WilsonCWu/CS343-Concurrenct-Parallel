#include "MPRNG.h"
#include "q1printer.h"
#include "q1voter.h"
#include "q1tallyVotes.h"
#include <sstream>
#include <iostream>
#include <string>

MPRNG mprng;

using namespace std;
bool convert( int &val, char *buffer ) {		// convert C string to integer
	if (strcmp(buffer, "d") == 0) return true;
	std::stringstream ss( buffer );			// connect stream and buffer
	string temp;
	ss >> dec >> val;					// convert integer from buffer
	return ! ss.fail() &&				// conversion successful ?
		! ( ss >> temp );				// characters after conversion all blank ?
} // convert

void usage( char *argv[] ) {
	cerr << "Usage: " << argv[0]
		<< " [ voters | 'd' [ group | 'd' [ votes | 'd' [ seed | 'd' [ processors | 'd' ] ] ] ] ]" << endl;
	exit( EXIT_FAILURE );				// TERMINATE
} // usage

int main (int argc, char *argv[]) {
	int voters = 6, group = 3, votes = 1, seed = getpid(), processors = 1;

	switch (argc) {
		case 6:
			if (!convert(processors, argv[5]) || processors <= 0) usage(argv);
		case 5:
			if (!convert(seed, argv[4]) || seed <= 0) usage(argv);
		case 4:
			if (!convert(votes, argv[3]) || votes <= 0) usage(argv);
		case 3:
			if (!convert(group, argv[2]) || group <= 0) usage(argv);
		case 2:
			if (!convert(voters, argv[1]) || voters <= 0) usage(argv);
		case 1:
			break;
		default:
			usage(argv);
	} // switch

	mprng.set_seed(seed);

	uProcessor p[processors - 1]; // number of kernel threads

	Printer printer(voters);
	TallyVotes voteTallier(voters, group, printer);
	Voter *tour[voters];

	/* create tasks */
	for (int i = 0; i < voters; i++) {
		tour[i] = new Voter(i, votes, voteTallier, printer);
	}

	/* wait for tasks */
	for (int i = 0; i < voters; i++) {
		delete tour[i];
	}

} // main
