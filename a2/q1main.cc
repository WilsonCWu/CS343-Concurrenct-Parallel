#include <iostream>
#include <sstream>
#include <fstream>
#include "q1binsertsort.h"

using namespace std;

/* *********** NON - BASIC - TYPE *********** */
class CharInt {
	char c;
	int i;
	public:
	CharInt() : c{'\0'}, i{0} {}
	CharInt(char c, int i) : c{c}, i{i} {}
	char getC() const { return c; }
	int getI() const { return i; }
};
/******************** END ******************* */


/* *********** OVERLOAD OPERATORS *********** */
bool operator==(const CharInt &ci1, const CharInt &ci2) {
	return ci1.getC() == ci2.getC() && ci1.getI() == ci2.getI();
}

bool operator<(const CharInt &ci1, const CharInt &ci2) {
	return ci1.getC() < ci2.getC()
		|| (ci1.getC() == ci2.getC() && ci1.getI() < ci2.getI());
}

ostream &operator<<(ostream &out, const CharInt &ci) {
	return out << ci.getC() << ci.getI();
}

istream &operator>>(istream &in, CharInt &ci) {
	char c;
	int i;
	in >> c;
	in >> i;
	ci = CharInt(c, i);
	return in;
}
/* ****************** END ******************* */


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
	} // switch

	string line;
	while (getline(*in, line)) {
		istringstream iss(line);
		Binsertsort<TYPE> btree;

		int n;
		iss >> n;
		/* read in values*/
		for (int i = 0; i < n; i++) {
			TYPE element;
			iss >> element;
			*out << element;
			if (i != n-1) *out << " ";
			btree.sort(element);
		} // for

		// Indicate read finished
		_Resume Binsertsort<TYPE>::Sentinel {} _At btree;
		*out << endl;

		/* retrieve values */
		try {
			_Enable {
				*out << btree.retrieve();
				while (true) {
					TYPE data = btree.retrieve();
					*out << " " << data;
				} // while
			} // _Enable
		} catch (Binsertsort<TYPE>::Sentinel &e) {
			*out << endl;
		} // try

		cout << endl;

	} // while

	delete in;
	if (out != &cout) delete out;
} // main
