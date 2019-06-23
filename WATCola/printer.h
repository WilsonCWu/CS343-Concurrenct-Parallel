#ifndef __PRINTER_H__
#define __PRINTER_H__


_Monitor Printer {
    struct PrintState {
        char state;
        int len;
        int values[2];
    };
    unsigned int numStudents, numVendingMachines, numCouriers;
    unsigned int last = 0; // last non-empty buffer index to print
    PrintState *printState;
    unsigned int id2Index(unsigned int kind, unsigned int lid);
    void flush();
    void reset();
  public:
    enum Kind { Parent, Groupoff, WATCardOffice, NameServer, Truck, BottlingPlant, Student, Vending, Courier };
    Printer(unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers);
    void print(Kind kind, char state);
    void print(Kind kind, char state, int value1);
    void print(Kind kind, char state, int value1, int value2);
    void print(Kind kind, unsigned int lid, char state);
    void print(Kind kind, unsigned int lid, char state, int value1);
    void print(Kind kind, unsigned int lid, char state, int value1, int value2);
    ~Printer();
};


#endif // __PRINTER_H__
