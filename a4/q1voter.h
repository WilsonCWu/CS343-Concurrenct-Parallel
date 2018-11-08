#ifndef __VOTER_H__
#define __VOTER_H__

#include "MPRNG.h"
#include "q1tallyVotes.h"

extern MPRNG mprng;

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

#endif // __VOTER_H__
