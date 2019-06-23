#ifndef __VENDINGMACHINE_H__
#define __VENDINGMACHINE_H__

#include "printer.h"
#include "nameserver.h"
#include "watcard.h"

_Task VendingMachine {
    Printer &prt;
    NameServer &nameServer;
    unsigned int id;
    unsigned int sodaCost;
    unsigned int *inv;
    void main();
  public:
    enum Flavours {
        ChinaDry,
        Schmilblick,
        Maluuba 
    };                                        // flavours of soda
    _Event Free {};                           // free, advertisement
    _Event Funds {};                          // insufficient funds
    _Event Stock {};                          // out of stock for particular flavour
    VendingMachine(Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost);
    void buy(Flavours flavour, WATCard &card);
    unsigned int *inventory();
    void restocked();
    _Nomutex unsigned int cost();
    _Nomutex unsigned int getId();
    ~VendingMachine();
};


#endif // __VENDINGMACHINE_H__
