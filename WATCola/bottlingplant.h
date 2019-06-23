#ifndef __BOTTLINGPLANT_H__
#define __BOTTLINGPLANT_H__

#include "printer.h"
#include "nameserver.h"

const int NUM_FLAVOUR = 3;

_Task BottlingPlant {
    Printer &prt;
    NameServer &nameServer;
    unsigned int numVendingMachines, maxShippedPerFlavour, maxStockPerFlavour, timeBetweenShipments;
    bool shutdown;
    unsigned int stock[NUM_FLAVOUR];
    void main();
  public:
    _Event Shutdown {};                       // shutdown plant
    BottlingPlant(Printer & prt, NameServer & nameServer, unsigned int numVendingMachines,
                 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
                 unsigned int timeBetweenShipments);
    void getShipment(unsigned int cargo[]);
    ~BottlingPlant();
};


#endif // __BOTTLINGPLANT_H__
