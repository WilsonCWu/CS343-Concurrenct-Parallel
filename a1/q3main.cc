#include <iostream>
#include <sstream>
#include <fstream>

#include "q3listing.h"

using namespace std;

void usage (char *argv[]) {
	cerr << "Usage: " << argv[0] << " [ infile ]" << endl;
	exit(EXIT_FAILURE);
}

int main (int argc, char * argv[]) {
	istream *in = &cin;

	switch (argc) {
		case 2:
			try {
				in = new ifstream(argv[1]);
			} catch (uFile::Failure &f) {
				cerr << "Error! Could not open output file \"" << argv[1] << "\"" << endl;
				usage(argv);
			} // try
		case 1:
			break;
		default:
			usage(argv);
	}

	string line;
	while (getline(*in, line)) {
		cout << "'" << line << "' : ";			// print the line
		if (line.length() == 0) {				// check empty line
			cout << "Warning! Blank line." << endl;
			continue;
		} // if
		istringstream iss(line);
		iss >> noskipws;						// no skip whitespace
		cout << "'";
		try{
			_Enable {
				Listing listing;
				char c;
				while (iss >> c) listing.next(c);
				listing.next('\n');				// indicate end of line
			} // _Enable
		} catch (Listing::Error &e) {
			string extra;
			getline(iss, extra);				// put rest of string to extra
			cout<<"' no";
			if (extra.length()) {
				cout << ", extraneous characters '"<<extra<<"'";
			} // if
		} catch (Listing::Match &m) {
			cout << "' yes";
		} // try
		cout<<endl;
	} // while

	if (in != &cin) delete in;
}

