#include "truck.h"
#include "vendingmachine.h"
#include "MPRNG.h"

extern MPRNG mprng;

enum {
    minWait = 1,
    maxWait = 10
};


Truck::Truck(Printer &prt, NameServer &nameServer, BottlingPlant &plant,
        unsigned int numVendingMachines, unsigned int maxStockPerFlavour):
    prt(prt), nameServer(nameServer), plant(plant),
    lastRestocked(numVendingMachines - 1),             // first one to restock is VM0
    numVendingMachines(numVendingMachines),
    maxStockPerFlavour(maxStockPerFlavour) {

    vendingMachines = nameServer.getMachineList();
    for (unsigned int i = 0; i < NUM_FLAVOUR; ++i) {
        cargo[i] = 0;
    } // for
}


/*
 * Returns true if cargo is empty
 */
bool Truck::emptyCargo() {
    for (unsigned int j = 0; j < NUM_FLAVOUR; ++j) {
        if (cargo[j] > 0) {
            return false;
        } // if
    } // for
    return true;
}

/*
 * Returns the total amount of cargo
 */
unsigned int Truck::totalCargo() {
    unsigned int sum = 0;
    for (unsigned int j = 0; j < NUM_FLAVOUR; ++j) {
        sum += cargo[j];
    } // for
    return sum;
}


void Truck::main() {
    prt.print(Printer::Truck, 'S');

    try {
        _Enable {
            for (;;) {
                _Accept(~Truck) {
                    break;
                } _Else {
                    yield(mprng(minWait, maxWait));

                    plant.getShipment(cargo);
                    prt.print(Printer::Truck, 'P', totalCargo());

                    for (unsigned int i = 0; i < numVendingMachines; ++i) {
                        if (emptyCargo()) break;               // check if cargo is empty

                        unsigned int vmId = (lastRestocked + 1) % numVendingMachines; // get VM's ID
                        prt.print(Printer::Truck, 'd', vmId, totalCargo());

                        unsigned int *inv = vendingMachines[vmId]->inventory();
                        unsigned int notReplenished = 0;
                        for (unsigned int j = 0; j < NUM_FLAVOUR; ++j) {
                            unsigned int space = maxStockPerFlavour - inv[j];
                            if (cargo[j] >= space) {           // have enough cargo to top-up the flavour
                                inv[j] += space;
                                cargo[j] -= space;
                            } else {                           // not enough to top-up the flavour
                                notReplenished += space - cargo[j];
                                inv[j] += cargo[j];
                                cargo[j] = 0;
                            } // if
                        } // for

                        if (notReplenished > 0) {              // unsuccessfully filled VM
                            prt.print(Printer::Truck, 'U', vmId, notReplenished);
                        } // if

                        prt.print(Printer::Truck, 'D', vmId, totalCargo());
                        lastRestocked = vmId;
                        vendingMachines[vmId]->restocked();
                    } // for
                }
            } // for
        } // _Enable
    } catch (BottlingPlant::Shutdown &) {}

    prt.print(Printer::Truck, 'F');
}


Truck::~Truck() {}
