#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include "q2matrixmultiply.h"

using namespace std;

bool convert( int &val, char *buffer ) {		// convert C string to integer
	std::stringstream ss( buffer );			// connect stream and buffer
	string temp;
	ss >> dec >> val;					// convert integer from buffer
	return ! ss.fail() &&				// conversion successful ?
		! ( ss >> temp );				// characters after conversion all blank ?
} // convert

void usage( char *argv[] ) {
	cerr << "Usage: " << argv[0]
		<< " xrows xcols-yrows ycols [ processors | X-matrix-file Y-matrix-file ]" << endl;
	exit( EXIT_FAILURE );				// TERMINATE
} // usage

int main (int argc, char *argv[]) {
	int processors = 1;
	int xrows, xycols, ycols;
	ifstream *finX, *finY;

	switch (argc) {
		case 6:
			try {
				finY = new ifstream(argv[5]);
			} catch (uFile::Failure &f) {
				cerr << "Error! Could not open Y-matrix input file \"" << argv[5] << "\"" << endl;
				usage(argv);
			} // try
			try {
				finX = new ifstream(argv[4]);
			} catch (uFile::Failure &f) {
				cerr << "Error! Could not open X-matrix input file \"" << argv[4] << "\"" << endl;
				usage(argv);
			} // try
		case 5:
			if (argc != 6 && (!convert(processors, argv[4]) || processors < 1)) usage(argv);
		case 4:
			if (!convert(xrows, argv[1]) || xrows < 1 ||
				!convert(xycols, argv[2]) || xycols < 1 ||
				!convert(ycols, argv[3]) || ycols < 1) usage(argv);
			break;
		default:
			usage(argv);
	} // switch

	uProcessor p[processors - 1];  // number of kernel threads

	int **matrixX = new int *[xrows];
	int **matrixY = new int *[xycols];
	int **matrixZ = new int *[xrows];
	createMatrix(matrixZ, xrows, ycols);

	if (argc == 6) {
		/* read matrix from file */
		readMatrix(finX, matrixX, xrows, xycols);
		readMatrix(finY, matrixY, xycols, ycols);
		delete finX;
		delete finY;

		/* do calculation */
		matrixmultiply(matrixZ, matrixX, xrows, xycols, matrixY, ycols);

		/* print to stdout */
		printMatrix(matrixX, matrixY, matrixZ, xrows, xycols, ycols);

	} else {
		/* create default matrix */
		createMatrix(matrixX, xrows, xycols);
		createMatrix(matrixY, xycols, ycols);

		/* do calculation */
		matrixmultiply(matrixZ, matrixX, xrows, xycols, matrixY, ycols);
	} // if

	/* clean up */
	for (int i = 0; i < xrows; i++) delete matrixX[i];
	for (int i = 0; i < xycols; i++) delete matrixY[i];
	for (int i = 0; i < xrows; i++) delete matrixZ[i];
	delete matrixX;
	delete matrixY;
	delete matrixZ;
} // main
