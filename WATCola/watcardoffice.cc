#include "watcardoffice.h"
#include "MPRNG.h"

extern MPRNG mprng;

/* ********************* _TASK Courier **************************** */
WATCardOffice::Job::Job(unsigned int sid, unsigned int amount):
       sid{sid}, amount{amount} {};

WATCardOffice::Courier::Courier(int id, WATCardOffice *office, Bank &bank, Printer &prt):
    id{id}, office{office}, bank{bank}, prt{prt} {};

WATCardOffice::Courier::~Courier() {}

void WATCardOffice::Courier::main() {
    prt.print(Printer::Courier, id, 'S');
    for (;;) {
        _Accept(~Courier) {
            break;
        } _Else {
            Job *job = office->requestWork();      // blocked in office, not busy waiting.
            if (job == nullptr) break;             // No job available

            unsigned int sid = job->sid;
            unsigned int amount = job->amount;
            WATCard *card = new WATCard();

            prt.print(Printer::Courier, id, 't', sid, amount);        // start funding
            bank.withdraw(sid, amount);

            if (mprng(0, 5) == 0) {                // 1/6 chance to lost student card.
                job->result.exception(new Lost);
                delete card;
                prt.print(Printer::Courier, id, 'L', sid);            // lost card
            } else {
                card->deposit(amount);
                job->result.delivery(card);
                prt.print(Printer::Courier, id, 'T', sid, amount);    // complete fund
            } // if
            delete job;
        }
    } // for
    prt.print(Printer::Courier, id, 'F');
}

/* ********************* _TASK WATCardOffice ********************** */
WATCardOffice::WATCardOffice(Printer &prt, Bank &bank, unsigned int numCouriers)
    :prt{prt}, bank{bank}, numCouriers{numCouriers} {
        couriers = new Courier *[numCouriers];
        for (unsigned int i = 0; i < numCouriers; i++) {
            couriers[i] = new Courier(i, this, bank, prt);
        } // for
}

void WATCardOffice::main() {
    prt.print(Printer::WATCardOffice, 'S');
    for (;;) {
        _Accept(~WATCardOffice) {
            for (;;) {
                _Accept(requestWork) {                     // Wake up waiting couriers
                } _Else break;
            } // for
            break;
        } or _When (jobs.size() < numCouriers) _Accept(create) {
            prt.print(Printer::WATCardOffice, 'C', sid, amount);
        } or _When (jobs.size() < numCouriers) _Accept(transfer) {
            prt.print(Printer::WATCardOffice, 'T', sid, amount);
        } or _When (!jobs.empty()) _Accept(requestWork) {
            prt.print(Printer::WATCardOffice, 'W');
        }
    } // for
    prt.print(Printer::WATCardOffice, 'F');
}

WATCard::FWATCard WATCardOffice::create(unsigned int sid, unsigned int amount) {
    Job *job = new Job(sid, amount);
    jobs.push_back(job);
    WATCardOffice::sid = sid;
    WATCardOffice::amount = amount;
    return job->result;
}

WATCard::FWATCard WATCardOffice::transfer(unsigned int sid, unsigned int amount, WATCard *card) {
    unsigned int newBalance = amount + card->getBalance();  // Add original balance
    delete card;
    Job *job = new Job(sid, newBalance);
    jobs.push_back(job);
    WATCardOffice::sid = sid;
    WATCardOffice::amount = amount;                         // Print only newly added amount
    return job->result;
}

WATCardOffice::Job *WATCardOffice::requestWork() {
    if (jobs.size() == 0) return nullptr;                   // No job while office is exiting

    Job *job = jobs.front();
    jobs.pop_front();
    return job;
}

WATCardOffice::~WATCardOffice() {
    for (unsigned int i = 0; i < numCouriers; i++) {
        delete couriers[i];
    } // for
    delete [] couriers;
}
