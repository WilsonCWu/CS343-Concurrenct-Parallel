#ifndef __TRUCK_H__
#define __TRUCK_H__

#include "printer.h"
#include "nameserver.h"
#include "bottlingplant.h"

_Task Truck {
    Printer &prt;
    NameServer &nameServer;
    BottlingPlant &plant;
    VendingMachine **vendingMachines;
    unsigned int lastRestocked;        // ID of most recently restocked VM
    unsigned int numVendingMachines;
    unsigned int maxStockPerFlavour;
    unsigned int cargo[NUM_FLAVOUR];
    unsigned int totalCargo();
    bool emptyCargo();
    void main();
    public:
    Truck(Printer &prt, NameServer &nameServer, BottlingPlant &plant,
            unsigned int numVendingMachines, unsigned int maxStockPerFlavour);
    ~Truck();
};


#endif // __TRUCK_H__
