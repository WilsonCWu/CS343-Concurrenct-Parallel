#include "q2tallyVotes.h"
#include "q2printer.h"
#include <cassert>
#include <iostream>
using namespace std;

TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: voters(voters), group(group), printer(printer) {
	}

void TallyVotes::wait() {
    bench.wait();                            // wait until signalled
    while ( rand() % 2 == 0 ) {              // multiple bargers allowed
		try{
        _Accept( vote, done ) {              // accept barging callers
        } _Else {                            // do not wait if no callers
        } // _Accept
		} catch (  uMutexFailure::RendezvousFailure & ) {}
    } // while
}

void TallyVotes::signalAll() {               // also useful
    while ( ! bench.empty() ) bench.signal();// drain the condition
}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
	if (voters - left < group) throw Failed();	// if not enough to form group, throw
	int ticket = serveTicket;

	while (out != 0) {						// may have barging
#ifdef OUTPUT
		printer.print(id, Voter::Barging);
#endif
		barger = 1;
		wait();
		barger = 0;
		if (voters - left < group) throw Failed();	// check enough votes
	}

	/* vote */
	count += 1;
	countPicture += ballot.picture;
	countStatue += ballot.statue;
	countGiftshop += ballot.giftshop;
#ifdef OUTPUT
	printer.print(id, Voter::States::Vote, ballot);
#endif

	if (count < group) {					// there are not enough voter yet
#ifdef OUTPUT
		printer.print(id, Voter::States::Block, count);
#endif
		while (ticket == serveTicket || out == 0) {	// wait until serve ticket increased, may have barging
			if (voters - left < group) {	// check enough votes
				count -= 1;
#ifdef OUTPUT
				printer.print(id, Voter::States::Unblock, count);
#endif
				throw Failed();
			}
			wait();
		}
		out -= 1;
#ifdef OUTPUT
		printer.print(id, Voter::States::Unblock, out);
#endif
	} else {									// there are enough voter
		out = group - 1;
		count = 0;
		serveTicket += 1;
		signalAll();
#ifdef OUTPUT
		printer.print(id, Voter::States::Complete);
#endif
	} // if

	/* vote */
	Tour result = countStatue > countPicture && countStatue > countGiftshop ? Tour::Statue
		: countPicture > countGiftshop ? Tour::Picture : Tour::GiftShop;

	if (out == 0) {							// last one need to erase the votes
		countStatue = 0;
		countPicture = 0;
		countGiftshop = 0;
		if (barger == 1) {
			signalAll();
		}
	}
	return result;
}

void TallyVotes::done() {
	left += 1;
	if (voters - left < group) signalAll();	// signal if not enough voters once I left.
}

