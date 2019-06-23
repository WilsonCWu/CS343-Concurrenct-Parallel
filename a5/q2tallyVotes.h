#ifndef __TALLY_VOTES_H__
#define __TALLY_VOTES_H__

#include <uBarrier.h>
#include <uSemaphore.h>
#include "AutomaticSignal.h"

_Monitor Printer;

#if defined( EXT )                           // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( INT )                         // internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
	uCondition cast;
    // private declarations for this kind of vote-tallier
#elif defined( INTB )                        // internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
    uCondition bench;                        // only one condition variable (you may change the variable name)
    void wait();                             // barging version of wait
    void signalAll();                        // unblock all waiting tasks
	int barger = 0;
#elif defined( AUTO )                        // automatic-signal monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
	AUTOMATIC_SIGNAL;
#elif defined( TASK )                        // internal/external scheduling task solution
_Task TallyVotes {
    // private declarations for this kind of vote-tallier
	uCondition cast, gate;
	void main();
#else
    #error unsupported voter type
#endif
	int serveTicket = 0;
    unsigned int voters, group;
	unsigned int out = 0, left = 0;
	unsigned int waitSignal = 0, gateSignal = 0;
	Printer &printer;
	unsigned int count = 0, countPicture = 0, countStatue = 0, countGiftshop = 0;
  public:                            // common interface
    _Event Failed {};
    TallyVotes( unsigned int voters, unsigned int group, Printer & printer );
    struct Ballot { unsigned int picture, statue, giftshop; };
    enum Tour { Picture = 'p', Statue = 's', GiftShop = 'g' };
    Tour vote( unsigned int id, Ballot ballot );
    void done();
	~TallyVotes() {};
};

#endif // __TALLY_VOTES_H__
