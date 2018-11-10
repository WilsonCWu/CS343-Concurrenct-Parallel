#include <iostream>
#include "q2printer.h"
using namespace std;

Printer::Printer(unsigned int voters) : voters(voters) {
#ifdef OUTPUT
	/* print name of task */
	for (unsigned int i = 0; i < voters; i++) {
		cout << "V" << i << (voters - 1 == i ? "" : "\t");
	}
	cout << endl;

	/* print divider line */
	for (unsigned int i = 0; i < voters; i++) {
		cout << "*******" << (voters - 1 == i ? "" : "\t");
	}
	cout << endl;
#endif

	/* create state buffer */
	printStates = new PrintState [voters];
	reset();
}
void Printer::print(unsigned int id, Voter::States state) {
	if (printStates[id].state != '-') flush();
	max = id > max ? id : max; // remember the last element to print
	printStates[id].state = state;
}
void Printer::print(unsigned int id, Voter::States state, TallyVotes::Tour tour) {
	print(id, state);
	printStates[id].tour = tour;
}
void Printer::print(unsigned int id, Voter::States state, TallyVotes::Ballot ballot) {
	print(id, state);
	printStates[id].ballot = ballot;
}
void Printer::print(unsigned int id, Voter::States state, unsigned int numBlocked) {
	print(id, state);
	printStates[id].numBlocked = numBlocked;
}

/*
 * flush():
 *	print current buffer and flush the buffer
 */
void Printer::flush() {
	for (unsigned int i = 0; i <= max; i++) {
		PrintState ps = printStates[i];
		string end = (i == voters - 1) ? "" : "\t";
		switch (ps.state) {
			case 'S':
			case 'b':
			case 'C':
			case 'X':
			case 'T':
				cout << ps.state << end;
				break;
			case 'B':
			case 'U':
				cout << ps.state << " " << ps.numBlocked << end;
				break;
			case 'F':
				cout << ps.state << " " << (char)ps.tour << end;
				break;
			case 'V':
				cout << ps.state << " " << ps.ballot.picture
					<< "," << ps.ballot.statue << "," << ps.ballot.giftshop << end;
				break;
			default:
				cout << end;
		}
	}
	cout << endl;
	reset();
}

/*
 * reset():
 *	reset the state buffer
 */
void Printer::reset() {
	for (unsigned int i = 0; i < voters; i++) printStates[i].state = '-';
}

Printer::~Printer() {
	/* check if need another flush */
	for (unsigned int i = 0; i < voters; i++) {
		if (printStates[i].state != '-') {
			flush();
			break;
		}
	}

#ifdef OUTPUT
	/* print end message */
	cout << "*****************" << endl;
	cout << "All tours started" << endl;
#endif

	delete printStates;
}
