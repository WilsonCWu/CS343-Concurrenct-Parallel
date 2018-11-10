#ifndef __PRINTER_H__
#define __PRINTER_H__

#include "q2voter.h"

_Monitor Printer {
    struct PrintState {
		char state;
		TallyVotes::Tour tour;
		TallyVotes::Ballot ballot;
		unsigned int numBlocked;
	};
	unsigned int voters;
	PrintState *printStates;
	unsigned int max = 0;
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

#endif // __PRINTER_H__
