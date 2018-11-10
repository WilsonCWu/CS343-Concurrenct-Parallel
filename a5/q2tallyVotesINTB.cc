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
		wait();
		count -= 1;
#ifdef OUTPUT
		printer.print(id, Voter::States::Unblock, count);
#endif
	} else {									// there are enough voter
		out = group;
		count -= 1;
#ifdef OUTPUT
		printer.print(id, Voter::States::Complete);
#endif
	} // if
	if (voters - left < group) throw Failed();	// check enough votes

	/* vote */
	Tour result = countStatue > countPicture && countStatue > countGiftshop ? Tour::Statue
		: countPicture > countGiftshop ? Tour::Picture : Tour::GiftShop;

	if (count == 0) {							// last one need to erase the votes
		countStatue = 0;
		countPicture = 0;
		countGiftshop = 0;
	}
	return result;
}

void TallyVotes::done() {
	left += 1;
	if (voters - left < group) signalAll();
}

