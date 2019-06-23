#include "nameserver.h"


NameServer::NameServer(Printer &prt, unsigned int numVendingMachines, unsigned int numStudents):
    prt(prt), numVendingMachines(numVendingMachines) {

    studentVMId = new unsigned int[numStudents];
    for (unsigned int i = 0; i < numStudents; ++i) {
        studentVMId[i] = i % numVendingMachines;
    } // for

    vendingMachines = new VendingMachine *[numVendingMachines];
    numVMReg = 0;
}


void NameServer::main() {
    prt.print(Printer::NameServer, 'S');

    for (;;) {
        _Accept(~NameServer) {
            prt.print(Printer::NameServer, 'F');
            break;
        } or _Accept(VMregister) {
            prt.print(Printer::NameServer, 'R', numVMReg);
            ++numVMReg;
        } or _When(numVMReg == numVendingMachines) _Accept(getMachine) {
            prt.print(Printer::NameServer, 'N', currentStudent, studentVMId[currentStudent]);
            studentVMId[currentStudent] = (studentVMId[currentStudent] + 1) % numVendingMachines;
        } or _When(numVMReg == numVendingMachines) _Accept(getMachineList) {
        }
    } // for
}


void NameServer::VMregister(VendingMachine *vendingmachine) {
    vendingMachines[numVMReg] = vendingmachine;
}


VendingMachine *NameServer::getMachine(unsigned int id) {
    currentStudent = id;                                   // pass information in
    return vendingMachines[studentVMId[id]];
}


VendingMachine **NameServer::getMachineList() {
    return vendingMachines;
}


NameServer::~NameServer() {
    delete [] studentVMId;
    delete [] vendingMachines;
}
