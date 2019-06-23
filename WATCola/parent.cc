#include "parent.h"
#include "MPRNG.h"

extern MPRNG mprng;

enum {
    giftMin = 1,
    giftMax = 3
};

Parent::Parent(Printer & prt, Bank & bank, unsigned int numStudents, unsigned int parentalDelay)
    : prt(prt), bank(bank), numStudents(numStudents), parentalDelay(parentalDelay) {};

void Parent::main() {
    prt.print(Printer::Parent, 'S');

    for (;;) {
        _Accept(~Parent) {
            prt.print(Printer::Parent, 'F');
            break;
        } _Else {
            yield(parentalDelay);
            unsigned int student = mprng(0, numStudents - 1);
            unsigned int amount = mprng(giftMin, giftMax);

            prt.print(Printer::Parent, 'D', student, amount);
            bank.deposit(student, amount);
        }
    } // for
}

Parent::~Parent() {}
