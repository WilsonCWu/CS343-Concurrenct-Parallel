template<typename T> _Coroutine Binsertsort {
	T value;                        // communication: value being passed down/up the tree
	void main() {
		try { _Enable {} } catch (Sentinel &e) {
			_Resume Sentinel {} _At resumer();
			return;
		}
		T pivot = value;
		Binsertsort<T>less;
		Binsertsort<T>greater;
		try {
			_Enable {
				while (true) {
					suspend();
					if (value < pivot) less.sort(value);
					else greater.sort(value); // TODO: test this, equal case
				}
			}
		} catch (Sentinel &e) {
			_Resume Sentinel {} _At less;
			try {
				_Enable {
					while (true) {
						value = less.retrieve();
						suspend();
					}
				}
			} catch (Sentinel &e) {}
			value = pivot;
			suspend();
			_Resume Sentinel {} _At greater;
			try {
				_Enable {
					while (true) {
						value = greater.retrieve();
						suspend();
					}
				}
			} catch (Sentinel &e) {}
		} // read done

		_Resume Sentinel {} _At resumer();
		return ;
	}
	public:
	_Event Sentinel {};
	void sort( T value ) {          // value to be sorted
		Binsertsort::value = value;
		resume();
	}
	T retrieve() {                  // retrieve sorted value
		resume();
		return value;
	}
};

#include <iostream>
#include <sstream>
#include <fstream>

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

	} // while

	delete in;
	if (out != &cout) delete out;
}
