#include "bank.h"

Bank::Bank(unsigned int numStudents) {
    account = new unsigned int[numStudents];
    accountCond = new uCondition[numStudents];
    for (unsigned int i = 0; i < numStudents; i++) {
        account[i] = 0;
    } // for
}

void Bank::deposit(unsigned int id, unsigned int amount) {
    account[id] += amount;
    accountCond[id].signal();
}

void Bank::withdraw(unsigned int id, unsigned int amount) {
    while (account[id] < amount) { accountCond[id].wait(); }
    account[id] -= amount;
}

Bank::~Bank() {
    delete [] account;
    delete [] accountCond;
}
