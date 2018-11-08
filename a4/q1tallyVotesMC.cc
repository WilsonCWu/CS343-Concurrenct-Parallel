#include "q1tallyVotes.h"
#include "q1printer.h"
#include <cassert>

TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: voters(voters), group(group), printer(printer) {
	}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
	if (voters - left < group) {	// if not enough, throw
		throw Failed();
	} // if

	lock.acquire();
	if (waitSignal != 0 || gateSignal != 0 || count == group) { // if trying barging
#ifdef OUTPUT
		printer.print(id, Voter::States::Barging);
#endif
		gateLock.wait(lock);
		gateSignal -= 1;			// waked up
	} // if
	if (voters - left < group) {	// if not enough after weakup, throw
		lock.release();
		throw Failed();
	} // if

	/* vote */
	count += 1;
	assert(count <= group);
	countPicture += ballot.picture;
	countStatue += ballot.statue;
	countGiftshop += ballot.giftshop;
#ifdef OUTPUT
	printer.print(id, Voter::States::Vote, ballot);
#endif

	if (count != group) {			// need more vote
#ifdef OUTPUT
		printer.print(id, Voter::States::Block, count);
#endif
		if (!gateLock.empty()) {	// check if anyone waiting on the gate
			gateSignal += 1;
			gateLock.signal();
		} // if
		waitLock.wait(lock);		// sleep until enough votes
		waitSignal -= 1;
		count -= 1;
#ifdef OUTPUT
		printer.print(id, Voter::States::Unblock, count);
#endif
		if (voters - left < group) {// if not enough voters after weakup throw
			lock.release();
			throw Failed();
		} // if
	} else {						// I am the last voter
#ifdef OUTPUT
		printer.print(id, Voter::States::Complete);
#endif
		count -= 1;
	} // if

	/* compute the tour result */
	Tour result = countStatue > countPicture && countStatue > countGiftshop ? Tour::Statue
		: countPicture > countGiftshop ? Tour::Picture : Tour::GiftShop;

	if (!waitLock.empty()) {		// if exist voter is waiting
		waitLock.signal();
		waitSignal += 1;
	} else {						// if all other voters are finished, cleanup
		count = 0;
		countStatue = 0;
		countPicture = 0;
		countGiftshop = 0;
		if (!gateLock.empty()) {	// if anyone waiting on the gate, wake it up
			gateLock.signal();
			gateSignal += 1;
		} // if
	}
	lock.release();
	return result;
}

void TallyVotes::done() {
	/* if not enough voters, just return */
	if (voters - left < group) return;

	lock.acquire();
	if (waitSignal != 0 || gateSignal != 0) { // if barging
		gateLock.wait(lock);
		gateSignal -= 1;
	} // if
	left += 1;

	/* if not enough voter after I left */
	if (voters - left < group) {
		gateLock.broadcast();
		waitLock.broadcast();
	} else if (waitSignal == 0 && gateSignal == 0) {
		if (!gateLock.empty()) {	// wakeup if anyone wait on gate
			gateLock.signal();
			gateSignal += 1;
		} // if
	} // if
	lock.release();
}

