#include "q1tallyVotes.h"
#include "q1printer.h"
#include <cassert>

TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: uBarrier(group), voters(voters), group(group), printer(printer) {
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

	if (waiters() + 1 < group) {				// there are not enough voter yet
#ifdef OUTPUT
		printer.print(id, Voter::States::Block, count);
#endif
		block();
		count -= 1;
#ifdef OUTPUT
		printer.print(id, Voter::States::Unblock, count);
#endif
	} else {									// there are enough voter
		block();
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
	if (voters - left == group && waiters() != 0) block(); // need to block for others to exit
	left += 1;
}
