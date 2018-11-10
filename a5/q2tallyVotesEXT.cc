#include "q2tallyVotes.h"
#include "q2printer.h"
#include <cassert>
#include <iostream>
using namespace std;

TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: voters(voters), group(group), printer(printer) {
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
		try {
			_Accept(vote, done);
		} catch (  uMutexFailure::RendezvousFailure & ) {}

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
	} // if
	return result;
}

void TallyVotes::done() {
	if (voters - left < group) return;
	left += 1;
	out -= 1;
	if (voters - left - out < group && out != 0) {
		try {
			_Accept(done, vote);
		} catch (  uMutexFailure::RendezvousFailure & ) {}
	}
}

