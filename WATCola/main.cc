#include <iostream>
#include <sstream>
#include <string>
#include "MPRNG.h"
#include "bank.h"
#include "bottlingplant.h"
#include "configparms.h"
#include "groupoff.h"
#include "nameserver.h"
#include "parent.h"
#include "printer.h"
#include "student.h"
#include "vendingmachine.h"
#include "watcardoffice.h"

using namespace std;

MPRNG mprng;

bool convert(int &val, char *buffer) {    // convert C string to integer
    std::stringstream ss(buffer);         // connect stream and buffer
    string temp;
    ss >> dec >> val;                     // convert integer from buffer
    return ! ss.fail() &&                 // conversion successful ?
        ! (ss >> temp);                   // characters after conversion all blank ?
} // convert

void usage(char *argv[]) {
    cerr << "Usage: " << argv[0]
        << " [ config-file [ random-seed (> 0) ] ]" << endl;
    exit(EXIT_FAILURE);                    // TERMINATE
} // usage

int main (int argc, char *argv[]) {
    int seed = getpid();
    ConfigParms configParms;

    switch (argc) {
        case 3:
            if (!convert(seed, argv[2]) || seed <= 0) usage(argv);
        case 2:
            processConfigFile(argv[1], configParms);
            break;
        case 1:
            processConfigFile("soda.config", configParms);
            break;
        default:
            usage(argv);
    } // switch

    mprng.set_seed(seed);

	/* initialize all objects */
    Printer prt{configParms.numStudents, configParms.numVendingMachines, configParms.numCouriers};

    Bank bank{configParms.numStudents};

    Parent parent{prt, bank, configParms.numStudents, configParms.parentalDelay};

    WATCardOffice office{prt, bank, configParms.numCouriers};

    Groupoff groupoff{prt, configParms.numStudents, configParms.sodaCost, configParms.groupoffDelay};

    NameServer nameserver{prt, configParms.numVendingMachines, configParms.numStudents};

    VendingMachine *vms[configParms.numVendingMachines];
    for (unsigned int i = 0; i < configParms.numVendingMachines; i++) {
        vms[i] = new VendingMachine{prt, nameserver, i, configParms.sodaCost};
    } // for

    BottlingPlant *bottlingplant = new BottlingPlant{prt, nameserver, configParms.numVendingMachines, configParms.maxShippedPerFlavour,
            configParms.maxStockPerFlavour, configParms.timeBetweenShipments};

    Student *students[configParms.numStudents];
    for (unsigned int i = 0; i < configParms.numStudents; i++) {
        students[i] = new Student{prt, nameserver, office, groupoff, i, configParms.maxPurchases};
    } // for

	/* wait for threads done */
    for (unsigned int i = 0; i < configParms.numStudents; i++) {
        delete students[i];
    } // for

    delete bottlingplant;

    for (unsigned int i = 0; i < configParms.numVendingMachines; i++) {
        delete vms[i];
    } // for
}
