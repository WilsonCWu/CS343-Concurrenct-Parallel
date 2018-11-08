#include "q1tallyVotes.h"
#include "q1printer.h"
#include <cassert>

TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: voters(voters), group(group), printer(printer) {
	}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
	if (voters - left < group) { // if not enough voters, throw
		throw Failed();
	} // if
	gateSem.P();
	if (voters - left < group) { // if not enough voters after weakup, throw
		gateSem.V();
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

	if (count != group) {		// not enough voter now
#ifdef OUTPUT
		printer.print(id, Voter::States::Block, count);
#endif
		gateSem.V();
		waitSem.P();
		count -= 1;
#ifdef OUTPUT
		printer.print(id, Voter::States::Unblock, count);
#endif
		if (voters - left < group) {// if not enough voters after weakup, throw
			waitSem.V();
			throw Failed();
		} // if
	} else {					// enough to form a group
#ifdef OUTPUT
		printer.print(id, Voter::States::Complete);
#endif
		count -= 1;
	} // if

	/* compute result */
	Tour result = countStatue > countPicture && countStatue > countGiftshop ? Tour::Statue
		: countPicture > countGiftshop ? Tour::Picture : Tour::GiftShop;

	if (count != 0) {			// some one havent get out from wait, wait them for exit
		waitSem.V();
		leftSem.P();
		if (leftSem.empty()) {	// all exit successful, V gate to let them in
			gateSem.V();
		} else {				// wakeup voter to exit
			leftSem.V();
		} // if
	} else {					// if I am the last one out, clean vote
		countStatue = 0;
		countPicture = 0;
		countGiftshop = 0;
		leftSem.V();
	} // if
	return result;
}

void TallyVotes::done() {
	if (voters - left < group) {	// if not enough to form group, return
		if (gateSem.counter() != 1) gateSem.V();
		if (!waitSem.empty()) waitSem.V();
		return;
	} // if
	doneSem.P();
	left += 1;
	if (voters - left < group) {	// if not enough to form group after I gone, wake all them up.
		if (!waitSem.empty()) waitSem.V();
	} // if
		if (gateSem.counter() != 1) gateSem.V();
	doneSem.V();
}

