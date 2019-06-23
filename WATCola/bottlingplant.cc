#include "bottlingplant.h"
#include "truck.h"
#include "MPRNG.h"

extern MPRNG mprng;

BottlingPlant::BottlingPlant(Printer & prt, NameServer & nameServer,
        unsigned int numVendingMachines, unsigned int maxShippedPerFlavour,
        unsigned int maxStockPerFlavour, unsigned int timeBetweenShipments):
    prt{prt}, nameServer{nameServer}, numVendingMachines{numVendingMachines},
    maxShippedPerFlavour{maxShippedPerFlavour}, maxStockPerFlavour{maxStockPerFlavour},
    timeBetweenShipments{timeBetweenShipments} {
    shutdown = false;
}

void BottlingPlant::main() {
    prt.print(Printer::BottlingPlant, 'S');
    bool waitForPickup = false;
    Truck *truck = new Truck(prt, nameServer, *this, numVendingMachines, maxStockPerFlavour);
    for (;;) {
        _Accept(~BottlingPlant) {
            shutdown = true;
            try {
                for (;;) {                            // wait for the truck finish last shipment and stop
                    _Accept (getShipment) {} _Else {}
                } // for
            } catch (uMutexFailure::RendezvousFailure &) {}
            delete truck;
            break;
        } or _When (waitForPickup) _Accept(getShipment) {
            prt.print(Printer::BottlingPlant, 'P');
            waitForPickup = false;
        } _When (!waitForPickup) _Else {
            yield(timeBetweenShipments);
            unsigned int totalBottles = 0;
            for (int i = 0; i < NUM_FLAVOUR; i++) {    // generate bottles
                unsigned int numBottles = mprng(0, maxShippedPerFlavour);
                stock[i] = numBottles;
                totalBottles += numBottles;
            } // for
            prt.print(Printer::BottlingPlant, 'G', totalBottles);
            waitForPickup = true;
        }
    } // for
    prt.print(Printer::BottlingPlant, 'F');
}

void BottlingPlant::getShipment(unsigned int cargo[]) {
    if (shutdown) _Throw Shutdown();
    for (int i = 0; i < NUM_FLAVOUR; i++) {
        cargo[i] = stock[i];
    } // for
}

BottlingPlant::~BottlingPlant() {}
