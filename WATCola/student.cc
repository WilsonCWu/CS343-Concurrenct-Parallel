#include "student.h"

extern MPRNG mprng;

enum {
    purchaseYieldMin = 1,
    purchaseYieldMax = 10,
    adTime = 4,
    initialAmt = 5
};

Student::Student(Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff,
    unsigned int id, unsigned int maxPurchases): prt(prt), nameServer(nameServer), cardOffice(cardOffice),
    groupoff(groupoff), id(id), maxPurchases(maxPurchases) {}


void Student::main() {
    unsigned int numPurchases = mprng(1, maxPurchases);
    VendingMachine::Flavours flavour = (VendingMachine::Flavours)mprng(0, 2);
    prt.print(Printer::Student, id, 'S', (int)flavour, (int)numPurchases);

    WATCard::FWATCard watcard = cardOffice.create(id, initialAmt);
    WATCard::FWATCard giftcard = groupoff.giftCard();
    VendingMachine *vm = nameServer.getMachine(id);
    prt.print(Printer::Student, id, 'V', vm->getId());
    bool giftcard_used = false;

    // Purchases soda
    for (unsigned int i = 0; i < numPurchases; ++i) {
        yield(mprng(purchaseYieldMin, purchaseYieldMax));

        for (;;) {
            bool giftcard_purchase = false;
            try {
                _Enable {
                    _Select(watcard || giftcard);
                    if (giftcard.available()) {                // giftcard is available
                        giftcard_purchase = true;
                        vm->buy(flavour, *giftcard);
                        giftcard_used = true;
                        delete giftcard;
                        giftcard.reset();                      // when Free is not caught, reset giftcard after use
                        prt.print(Printer::Student, id, 'G', (int)flavour, 0);
                    } else {                                   // watcard is available
                        vm->buy(flavour, *watcard);
                        prt.print(Printer::Student, id, 'B', (int)flavour, (*watcard).getBalance());
                    } // if
                } // _Enable
                break;
            } catch (WATCardOffice::Lost &) {
                prt.print(Printer::Student, id, 'L');
                watcard = cardOffice.create(id, initialAmt);
            } catch (VendingMachine::Funds &) {
                watcard = cardOffice.transfer(id, vm->cost() + initialAmt, watcard);
                yield(mprng(purchaseYieldMin, purchaseYieldMax));
            } catch (VendingMachine::Stock &) {
                vm = nameServer.getMachine(id);
                prt.print(Printer::Student, id, 'V', vm->getId());
                yield(mprng(purchaseYieldMin, purchaseYieldMax));
            } catch (VendingMachine::Free &) {
                if (giftcard_purchase) {
                    prt.print(Printer::Student, id, 'a', (int)flavour, (*giftcard).getBalance());
                } else {
                    prt.print(Printer::Student, id, 'A', (int)flavour, (*watcard).getBalance());
                } // if
                yield(adTime);
                yield(mprng(purchaseYieldMin, purchaseYieldMax));
            } // try
        } // for
    } // for
    try {    // In case watcard fund() -> giftcard available -> watcard lost() -> finished purchase
        delete watcard();
    } catch (WATCardOffice::Lost &) {}
    if (!giftcard_used) delete giftcard();
    prt.print(Printer::Student, id, 'F');
}

