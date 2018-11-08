#ifndef __TALLY_VOTES_H__
#define __TALLY_VOTES_H__

#include <uBarrier.h>
#include <uSemaphore.h>

_Monitor Printer;

#if defined( MC )                    // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
	uOwnerLock lock;
	uCondLock gateLock;
	uCondLock waitLock;
#elif defined( SEM )                // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
	uSemaphore gateSem{1}, waitSem{0}, leftSem{0}, doneSem{1};
#elif defined( BAR )                // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    unsigned int voters, group;
	unsigned int left = 0;
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
};

#endif // __TALLY_VOTES_H__
