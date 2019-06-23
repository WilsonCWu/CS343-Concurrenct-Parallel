#ifndef __PARENT_H__
#define __PARENT_H__

#include "printer.h"
#include "bank.h"

_Task Parent {
    Printer &prt;
    Bank &bank;
    unsigned int numStudents, parentalDelay;
    void main();
  public:
    Parent(Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay);
    ~Parent();
};


#endif // __PARENT_H__
