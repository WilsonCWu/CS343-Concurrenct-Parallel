#include "groupoff.h"
#include "MPRNG.h"
extern MPRNG mprng;

Groupoff::Groupoff(Printer &prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay)
    :prt{prt}, numStudents{numStudents}, sodaCost{sodaCost}, groupoffDelay{groupoffDelay} {
        assignedCount = 0;
        assigned = new bool[numStudents];
        for (unsigned int i = 0; i < numStudents; i++) {
            assigned[i] = false;
        } // for
        fcards = new WATCard::FWATCard[numStudents];
}

void Groupoff::main() {
    prt.print(Printer::Groupoff, 'S');
    for (;;) {
        _When (allAssigned && assignedCount == 0) _Accept(~Groupoff) {
            break;
        } or _When(!allAssigned) _Accept(giftCard) {
            assignedCount += 1;
            allAssigned = (assignedCount == numStudents);    // set to true when start deposit to watcard
        } _When (allAssigned) _Else {
            if (assignedCount == 0) break;
            yield(groupoffDelay);
            unsigned int nth = mprng(0, assignedCount-1);
            for (unsigned int i = 0; i < numStudents; i++) {
                if (assigned[i]) continue;
                if (nth == 0) {                              // assigned to nth unempty fcards
                    assigned[i] = true;
                    WATCard *card = new WATCard();
                    card->deposit(sodaCost);
                    fcards[i].delivery(card);
                    prt.print(Printer::Groupoff, 'D', sodaCost);
                    break;
                } // if
                nth -= 1;
            } // for
            assignedCount -= 1;
        }
    } // for
    prt.print(Printer::Groupoff, 'F');
}

WATCard::FWATCard Groupoff::giftCard() {
    return fcards[assignedCount];
}

Groupoff::~Groupoff() {
    delete [] assigned;
    delete [] fcards;
}
