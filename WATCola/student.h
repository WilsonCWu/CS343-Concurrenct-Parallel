#ifndef __STUDENT_H__
#define __STUDENT_H__

#include "MPRNG.h"
#include "printer.h"
#include "groupoff.h"
#include "nameserver.h"
#include "watcardoffice.h"
#include "vendingmachine.h"

_Task Student {
    Printer &prt;
    NameServer &nameServer;
    WATCardOffice &cardOffice;
    Groupoff &groupoff;
    unsigned int id;
    unsigned int maxPurchases;
    void main();
  public:
    Student(Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff,
             unsigned int id, unsigned int maxPurchases);
};


#endif // __STUDENT_H__
