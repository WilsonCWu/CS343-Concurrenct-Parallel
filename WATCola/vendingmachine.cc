#include "vendingmachine.h"
#include "MPRNG.h"
#include "bottlingplant.h"

extern MPRNG mprng;

VendingMachine::VendingMachine(Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost) :
    prt(prt), nameServer(nameServer), id(id), sodaCost(sodaCost) {

    nameServer.VMregister(this);
    inv = new unsigned int[NUM_FLAVOUR];
    for (int i = 0; i < NUM_FLAVOUR; ++i) {
      inv[i] = 0;
    } // for
}


void VendingMachine::main() {
    prt.print(Printer::Vending, id, 'S', sodaCost);

    for (;;) {
        try {
            _Accept(~VendingMachine) {
                break;
            } or _Accept(buy) {
            } or _Accept(inventory) {
                prt.print(Printer::Vending, id, 'r');
                _Accept(restocked);
                prt.print(Printer::Vending, id, 'R');
            }
        } catch (uMutexFailure::RendezvousFailure &) {}
    } // for

    prt.print(Printer::Vending, id, 'F');
}


void VendingMachine::buy(Flavours flavour, WATCard &card) {
    if (card.getBalance() < sodaCost) {
        _Throw Funds();
    } else if (inv[flavour] == 0) {                   // No stock of flavour
        _Throw Stock();
    } else if (mprng(0,4) == 0) {                     // Free soda (1 in 5 chance)
        _Throw Free();
    }

    inv[flavour] -= 1;
    card.withdraw(sodaCost);
    prt.print(Printer::Vending, id, 'B', flavour, inv[flavour]);
}


unsigned int *VendingMachine::inventory() {
    return inv;
}


void VendingMachine::restocked() {}


_Nomutex unsigned int VendingMachine::cost() {
    return sodaCost;
}


_Nomutex unsigned int VendingMachine::getId() {
    return id;
}


VendingMachine::~VendingMachine() {
    delete [] inv;
}
