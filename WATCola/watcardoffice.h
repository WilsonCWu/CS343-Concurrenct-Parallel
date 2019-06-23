#ifndef __WATCARDOFFICE_H__
#define __WATCARDOFFICE_H__

#include <deque>
#include "printer.h"
#include "bank.h"
#include "watcard.h"

_Task WATCardOffice {
    struct Job {                              // marshalled arguments and return future
        unsigned int sid, amount;
        WATCard::FWATCard result;             // return future
        Job(unsigned int sid, unsigned int amount);
    };
    _Task Courier {
        int id;
        WATCardOffice *office;
        Bank &bank;
        Printer &prt;
        void main();
      public:
        Courier(int id, WATCardOffice *office, Bank &bank, Printer &prt);
        ~Courier();
    };                                        // communicates with bank

    Printer &prt;
    Bank &bank;
    unsigned int numCouriers;
    std::deque<Job *>jobs;
    Courier **couriers;

    /* communication variables for main routine */
    unsigned int sid, amount;

    void main();
  public:
    _Event Lost {};                           // lost WATCard
    WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers);
    WATCard::FWATCard create(unsigned int sid, unsigned int amount);
    WATCard::FWATCard transfer(unsigned int sid, unsigned int amount, WATCard *card);
    Job * requestWork();
    ~WATCardOffice();
};


#endif // __WATCARDOFFICE_H__
