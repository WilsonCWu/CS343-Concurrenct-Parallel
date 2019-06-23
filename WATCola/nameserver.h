#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

#include "printer.h"

_Task VendingMachine;

_Task NameServer {
    Printer &prt;
    unsigned int *studentVMId;        // VM id for each student on their next getMachine call
    VendingMachine **vendingMachines;
    unsigned int numVMReg;            // Number of registered vending machines
    unsigned int numVendingMachines;
    unsigned int currentStudent;      // Student ID of the caller of getMachine 
    void main();
  public:
    NameServer(Printer &prt, unsigned int numVendingMachines, unsigned int numStudents);
    void VMregister(VendingMachine *vendingmachine);
    VendingMachine *getMachine(unsigned int id);
    VendingMachine **getMachineList();
    ~NameServer();
};


#endif // __NAMESERVER_H__
