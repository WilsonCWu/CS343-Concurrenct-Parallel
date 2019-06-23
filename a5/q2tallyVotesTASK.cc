#include "q2tallyVotes.h"
#include "q2printer.h"
#include <cassert>
#include <iostream>
using namespace std;

TallyVotes::TallyVotes(unsigned int voters, unsigned int group, Printer & printer)
	: voters(voters), group(group), printer(printer) {
	}

TallyVotes::Tour TallyVotes::vote(unsigned int id, Ballot ballot) {
	if (voters < group) throw Failed();	// if not enough to form group, throw

	/* vote */
#ifdef OUTPUT
	printer.print(id, Voter::States::Vote, ballot);
#endif
	count += 1;
	countPicture += ballot.picture;
	countStatue += ballot.statue;
	countGiftshop += ballot.giftshop;

#ifdef OUTPUT
	printer.print(id, Voter::States::Block, count);
#endif
	cast.wait();
	count -= 1;
	if (voters < group && out == 0) throw Failed();	// check enough votes
#ifdef OUTPUT
	if (count == 0 && out == 1) {		// the first one need to print complete & compute msg
		printer.print(id, Voter::States::Complete);
	}
	printer.print(id, Voter::States::Unblock, count);
#endif

	/* vote */
	Tour result = countStatue > countPicture && countStatue > countGiftshop ? Tour::Statue
		: countPicture > countGiftshop ? Tour::Picture : Tour::GiftShop;

	return result;
}

void TallyVotes::done() {
}

void TallyVotes::main() {
	for (;;) {
		try {
			_Accept(~TallyVotes) {		// stop looping on destructor
				break;
			} or _Accept(done) {		// accept done on anytime
				voters -= 1;
				if (voters < group && !cast.empty()) cast.signalBlock();
			} or _When (out == 0 && count < group) _Accept(vote) {	// if not enough voters && not barging
				if (count == group) {	// enough to form a group
					out = 1;			// set out to 1 to indicate signal
					for (unsigned int i = 0; i < group; i++) {	// weak up all voters
						cast.signalBlock();
					}
					out = 0;			// reset cast
					countStatue = 0;
					countPicture = 0;
					countGiftshop = 0;
				}
			} //_Accept
		} catch (  uMutexFailure::RendezvousFailure & ) {}
	}
}
