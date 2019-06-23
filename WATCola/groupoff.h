#ifndef __GROUPOFF_H__
#define __GROUPOFF_H__

#include "printer.h"
#include "watcard.h"

_Task Groupoff {
    Printer &prt;
    unsigned int numStudents, sodaCost, groupoffDelay, assignedCount;
    WATCard::FWATCard *fcards;
    bool allAssigned = false;    // start fill in real watcard after all student started
    bool *assigned;
    void main();
  public:
    Groupoff(Printer & prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay);
    WATCard::FWATCard giftCard();
    ~Groupoff();
};


#endif // __GROUPOFF_H__
