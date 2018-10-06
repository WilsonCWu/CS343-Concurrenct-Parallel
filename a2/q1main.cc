#include <iostream>
#include <sstream>
#include <fstream>
#include "q1binsertsort.h"

using namespace std;

void usage (char *argv[]) {
	cerr << "Usage: " << argv[0] << "unsorted-file [ sorted-file ]" << endl;
	exit(EXIT_FAILURE);
}

int main (int argc, char *argv[]) {
	istream *in;
	ostream *out = &cout;

	switch (argc) {
		case 3:
			try {
				out = new ofstream(argv[2]);
			} catch (uFile::Failure &f) {
				cerr << "Error! Could not open output file \"" << argv[2] << "\"" << endl;
				usage(argv);
			} // try
		case 2:
			try {
				in = new ifstream(argv[1]);
			} catch (uFile::Failure &f) {
				cerr << "Error! Could not open input file \"" << argv[1] << "\"" << endl;
				usage(argv);
			} // try
			break;
		default:
			usage(argv);
	}

	string line;
	while (getline(*in, line)) {
		istringstream iss(line);
		Binsertsort<TYPE> btree;

		int n;
		iss >> n;
		for (int i = 0; i < n; i++) {
			TYPE element;
			iss >> element;
			*out << element;
			if (i != n-1) *out << " ";
			btree.sort(element);
		}

		// Indicate read finished
		_Resume Binsertsort<TYPE>::Sentinel {} _At btree;
		*out << endl;

		try {
			_Enable {
				*out << btree.retrieve();
				while (true) {
					TYPE data = btree.retrieve();
					*out << " " << data;
				}
			}
		} catch (Binsertsort<TYPE>::Sentinel &e) {
			*out << endl;
		}

		cout << endl;

	} // while

	delete in;
	if (out != &cout) delete out;
}
