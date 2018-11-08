#include "q1voter.h"
#include "q1printer.h"

void Voter::main() {
	yield(mprng(19));
	// vote for n times
	for (unsigned int i = 0; i < nvotes; i++) {
#ifdef OUTPUT
		printer.print(id, States::Start);
#endif
		yield(mprng(4));
		TallyVotes::Ballot ballot = cast();
		try {
			_Enable {
				TallyVotes::Tour tour = voteTallier.vote(id, ballot);
				yield(mprng(4));
#ifdef OUTPUT
				printer.print(id, States::Finished, tour);
#endif
			} // Enable
		} catch (TallyVotes::Failed &e) {
#ifdef OUTPUT
			printer.print(id, States::Failed); // not enough voters
#endif
			break;
		} // try
	} // for
	voteTallier.done();
#ifdef OUTPUT
	printer.print(id, States::Terminated);
#endif
} // main

Voter::Voter(unsigned int id, unsigned int nvotes, TallyVotes & voteTallier, Printer & printer)
	: id(id), nvotes(nvotes), voteTallier(voteTallier), printer(printer) {};
