#include "printer.h"

#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

const char EMPTY = '-';

/*
 * printNTimes:
 *    print a name num times, and append number from 0 to num-1 to each name
 *
 */
void printNTimes(string name, int num, bool last) {
    for (int i = 0; i < num; i++) {
        cout << name << i << (i == num - 1 && last ? "" : "\t");
    } // for
}

/*
 * id2Index:
 *    return the corresponding index in printState by given id
 *
 */
unsigned int Printer::id2Index(unsigned int kind, unsigned int id) {
    unsigned int index = kind + id;
    switch (kind) {
        case Courier:
            index += numVendingMachines - 1;
        case Vending:
            index += numStudents - 1;
    } // switch
    return index;
}

/*
 * reset:
 *    reset the printState to empty;
 *
 */
void Printer::reset() {
    for (unsigned int i = 0; i <= last; i++) {
        printState[i].state = EMPTY;
        printState[i].len = 0;
    } // for
    last = 0;
}

/*
 * flush:
 *    print the state buffer and reset printState
 *
 */
void Printer::flush() {
    for (unsigned int i = 0; i <= last; i++) {
        PrintState ps = printState[i];
        switch(ps.state) {
            case '-':
                cout << '\t';
                break;
            default:
                cout << ps.state;
                for (int l = 0; l < ps.len; l++) {
                    cout << ps.values[l] << (l == ps.len-1 ? "" : ",");
                } // for
                if (i != last) cout << "\t";
        }
    } // for
    cout << endl;
    reset();
}

Printer::Printer(unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers)
    :numStudents{numStudents}, numVendingMachines{numVendingMachines}, numCouriers{numCouriers} {
        /* 6: parent, gropoff, watoff, names, truck, plant */
        int total = 6 + numStudents + numVendingMachines + numCouriers;
        printState = new PrintState[total];
        last = total; // reset all state
        reset();

        /* print start message */
        cout << "Parent" << '\t'
            << "Gropoff" << '\t'
            << "WATOff" << '\t'
            << "Names" << '\t'
            << "Truck" << '\t'
            << "Plant" << '\t';
        printNTimes("Stud", numStudents, false);
        printNTimes("Mach", numVendingMachines, false);
        printNTimes("Cour", numCouriers, true);
        cout << endl;

        for (int i = 0; i < total; i++) {
            cout << "*******" << (i == total -1 ? "" : "\t");
        } // for
        cout << endl;
}

void Printer::print(Kind kind, char state) {
    unsigned int id = (unsigned int)kind;
    if (printState[id].state != EMPTY) { flush(); }
    last = id > last ? id : last; // remember the index of last element to print
    printState[id].state = state;
}

void Printer::print(Kind kind, char state, int value1) {
    print(kind, state);
    printState[kind].len += 1;
    printState[kind].values[0] = value1;
}

void Printer::print(Kind kind, char state, int value1, int value2) {
    print(kind, state, value1);
    printState[kind].len += 1;
    printState[kind].values[1] = value2;
}

void Printer::print(Kind kind, unsigned int lid, char state) {
    unsigned int id = id2Index(kind, lid);
    if (printState[id].state != EMPTY) { flush(); }
    last = id > last ? id : last; // remember the index of last element to print
    printState[id].state = state;
}

void Printer::print(Kind kind, unsigned int lid, char state, int value1) {
    unsigned int id = id2Index(kind, lid);
    print(kind, lid, state);
    printState[id].len += 1;
    printState[id].values[0] = value1;
}

void Printer::print(Kind kind, unsigned int lid, char state, int value1, int value2) {
    unsigned int id = id2Index(kind, lid);
    print(kind, lid, state, value1);
    printState[id].len += 1;
    printState[id].values[1] = value2;
}

Printer::~Printer() {
    /* check if buffer is empty */
    for (unsigned int i = 0; i <= last; i++) {
        if (printState[i].state != EMPTY) {
            flush();
            break;
        } // if
    } // for

    /* print end message */
    cout << "***********************" << endl;
    delete [] printState;
}
