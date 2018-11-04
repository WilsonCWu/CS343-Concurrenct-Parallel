#include "MPRNG.h"
MPRNG mprng;

_Monitor Printer;

/* *********************************************************************** */
#if defined( MC )                    // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
	uOwnerLock lock;
	uCondLock gateLock;
	uCondLock waitLock;
#elif defined( SEM )                // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( BAR )                // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    unsigned int voters, group;
	unsigned int left = 0;
	unsigned int waitSignal = 0, gateSignal = 0;
	Printer &printer;
	unsigned int count = 0, countPicture = 0, countStatue = 0, countGiftshop = 0;
  public:                            // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum Tour { Picture = 'p', Statue = 's', GiftShop = 'g' };
    Tour vote( unsigned int id, Ballot ballot );
    void done();
};
/* *********************************************************************** */

/* *********************************************************************** */
_Task Voter {
	unsigned int id, nvotes;
	TallyVotes &voteTallier;
	Printer &printer;
    TallyVotes::Ballot cast() {        // cast 3-way vote
        // O(1) random selection of 3 items without replacement using divide and conquer.
        static unsigned int voting[3][2][2] = { { {2,1}, {1,2} }, { {0,2}, {2,0} }, { {0,1}, {1,0} } };
        unsigned int picture = mprng( 2 ), statue = mprng( 1 );
        return (TallyVotes::Ballot){ picture, voting[picture][statue][0], voting[picture][statue][1] };
    }
	void main();
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
                   Complete = 'C', Finished = 'F', Failed = 'X', Terminated = 'T' };
    Voter( unsigned int id, unsigned int nvotes, TallyVotes & voteTallier, Printer & printer );
};
/* *********************************************************************** */

/* *********************************************************************** */
_Monitor Printer {
    struct PrintState {
		char state;
		TallyVotes::Tour tour;
		TallyVotes::Ballot ballot;
		unsigned int numBlocked;
	};
	unsigned int voters;
	PrintState *printStates;
	void flush();
	void reset();
  public:
    Printer( unsigned int voters );
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
    void print( unsigned int id, Voter::States state, TallyVotes::Ballot ballot );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
	~Printer();
};
/* *********************************************************************** */

/* ******************TALLY.CC********************************************* */
#include <cassert>
#if defined (MC)
TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: voters(voters), group(group), printer(printer) {
	}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
	if (voters - left < group) {
		throw Failed();
	}
	lock.acquire();
	//if (signal != 0 || count == group) gateLock.wait(lock);
	if (waitSignal != 0 || gateSignal != 0 || count == group) {
		printer.print(id, Voter::States::Barging);
		gateLock.wait(lock);
		gateSignal -= 1;	// waked up
	}
	if (voters - left < group) {
		lock.release();
		throw Failed();
	}
	count += 1;
	assert(count <= group);
	countPicture += ballot.picture;
	countStatue += ballot.statue;
	countGiftshop += ballot.giftshop;
	printer.print(id, Voter::States::Vote, ballot);
	if (count != group) {
		printer.print(id, Voter::States::Block, count);
		if (!gateLock.empty()) {
			gateSignal += 1;
			gateLock.signal();
		}
		waitLock.wait(lock);
		if (voters - left < group) {
			lock.release();
			throw Failed();
		}
		waitSignal -= 1;
		count -= 1;
		printer.print(id, Voter::States::Unblock, count);
	} else {
		printer.print(id, Voter::States::Complete);
	}
	Tour result = countStatue > countPicture && countStatue > countGiftshop ? Tour::Statue
		: countPicture > countGiftshop ? Tour::Picture : Tour::GiftShop;
	if (!waitLock.empty()) {
		waitLock.signal();
		waitSignal += 1;
	} else {
		count = 0;
		countStatue = 0;
		countPicture = 0;
		countGiftshop = 0;
		if (!gateLock.empty()) {
			gateLock.signal();
			gateSignal += 1;
		}
	}
	lock.release();
	return result;
}

void TallyVotes::done() {
	lock.acquire();
	if (waitSignal != 0 || gateSignal != 0) {
		gateLock.wait(lock);
		gateSignal -= 1;
	}
	left += 1;
	if (voters - left < group) {
		gateLock.broadcast();
		waitLock.broadcast();
	} else if (waitSignal == 0 && gateSignal == 0) {
		if (!gateLock.empty()) {
			gateLock.signal();
			gateSignal += 1;
		}
	}
	lock.release();
}

#endif
/* *********************************************************************** */

/* ******************VOTER.CC********************************************* */
using namespace std;
void Voter::main() {
	yield(mprng(19));
	for (int i = 0; i < nvotes; i++) {
		printer.print(id, States::Start);
		yield(mprng(4));
		TallyVotes::Ballot ballot = cast();
		try {
			_Enable {
				TallyVotes::Tour tour = voteTallier.vote(id, ballot);
				yield(mprng(4));
				printer.print(id, States::Finished, tour);
			}
		} catch (TallyVotes::Failed &e) {
			printer.print(id, States::Failed);
			break;
		}
	}
	printer.print(id, States::Terminated);
	voteTallier.done();
}
Voter::Voter(unsigned int id, unsigned int nvotes, TallyVotes & voteTallier, Printer & printer)
	: id(id), nvotes(nvotes), voteTallier(voteTallier), printer(printer) {};
/* *********************************************************************** */

/* *******************PRINTER.CC****************************************** */
#include <iostream>
using namespace std;

Printer::Printer(unsigned int voters) : voters(voters) {
	for (int i = 0; i < voters; i++) {
		cout << "V" << i << (voters == i ? "" : "\t");
	}
	cout << endl;

	for (int i = 0; i < voters; i++) {
		cout << "*******" << (voters == i ? "" : "\t");
	}
	cout << endl;

	printStates = new PrintState [voters];
	reset();
}
void Printer::print(unsigned int id, Voter::States state) {
	if (printStates[id].state != '-') flush();
	printStates[id].state = state;
}
void Printer::print(unsigned int id, Voter::States state, TallyVotes::Tour tour) {
	print(id, state);
	printStates[id].tour = tour;
}
void Printer::print(unsigned int id, Voter::States state, TallyVotes::Ballot ballot) {
	print(id, state);
	printStates[id].ballot = ballot;
}
void Printer::print(unsigned int id, Voter::States state, unsigned int numBlocked) {
	print(id, state);
	printStates[id].numBlocked = numBlocked;
}
void Printer::flush() {
	for (int i = 0; i < voters; i++) {
		PrintState ps = printStates[i];
		string end = (i == voters - 1) ? "" : "\t";
		switch (ps.state) {
			case 'S':
			case 'b':
			case 'C':
			case 'X':
			case 'T':
				cout << ps.state << end;
				break;
			case 'B':
			case 'U':
				cout << ps.state << " " << ps.numBlocked << end;
				break;
			case 'F':
				cout << ps.state << " " << (char)ps.tour << end;
				break;
			case 'V':
				cout << ps.state << " " << ps.ballot.picture
					<< "," << ps.ballot.statue << "," << ps.ballot.giftshop << end;
				break;
			default:
				cout << end;
		}
	}
	cout << endl;
	reset();
}
void Printer::reset() {
	for (int i = 0; i < voters; i++) printStates[i].state = '-';
}
Printer::~Printer() {
	for (int i = 0; i < voters; i++) {
		if (printStates[i].state != '-') {
			flush();
			break;
		}
	}
	cout << "*****************" << endl;
	cout << "All tours started" << endl;
	delete printStates;
}
/* *********************************************************************** */

#include <sstream>
#include <iostream>
#include <string>

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
	for (int i = 0; i < voters; i++) {
		tour[i] = new Voter(i, votes, voteTallier, printer);
	}

	for (int i = 0; i < voters; i++) {
		delete tour[i];
	}


} // main
