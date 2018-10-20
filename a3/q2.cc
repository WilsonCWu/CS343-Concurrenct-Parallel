#include <uCobegin.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

_Task Multiplier {
	int id;
	void main () {
		if (id >= xr) return;
		// create two tasks
		Multiplier left(id*2+1), right(id*2+2);

		// calculation
		for (int i = 0; i < yc; i++) {
			Z[id][i] = 0;
			for (int j = 0; j < xc; j++) {
				Z[id][i] += X[id][j] * Y[j][i];
			} // for
		} // for
	}
	public:
	static int xr, xc, yc, **X, **Y, **Z;
	Multiplier(int id): id(id) {};
};

// shared variables
int Multiplier::xr;
int Multiplier::xc;
int Multiplier::yc;
int **Multiplier::X;
int **Multiplier::Y;
int **Multiplier::Z;

void matrixmultiply(int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc)
#if defined ( EXPLICIT )
{
	Multiplier::xr = xr;
	Multiplier::xc = xc;
	Multiplier::yc = yc;
	Multiplier::X = X;
	Multiplier::Y = Y;
	Multiplier::Z = Z;
	{ // COBEGIN
		Multiplier m(0);
	} // COEND
}
#elif defined ( IMPLICIT )
{
	COFOR(row, 0, xr,
		for (unsigned int i = 0; i < yc; i++) {
			Z[row][i] = 0;
			for (unsigned int j = 0; j < xc; j++) {
				Z[row][i] += X[row][j] * Y[j][i];
			} // for
		} // for
	); // COFOR
}
#else
#error must define EXPLICIT or IMPLICIT
#endif

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

/*
 * read matrix from file
 */
void readMatrix(ifstream *fin, int *matrix[], unsigned int row, unsigned int col) {
	for (unsigned int i = 0; i < row; i++) {
		matrix[i] = new int [col];
		for (unsigned int j = 0; j < col; j++) *fin >> matrix[i][j];
	} // while
}


/*
 * create matrix with default entry value 37
 */
void createMatrix(int *matrix[], unsigned int row, unsigned int col) {
	while (row -- > 0) {
		matrix[row] = new int [col];
		for (unsigned int i = 0; i < col; i++) matrix[row][i] = 37;
	} // while
}


/*
 * print matrix
 */
void printMatrix(int *matrixX[], int *matrixY[], int *matrixZ[], int xrows, int xycols, int ycols) {
	/* print upper part */
	for (int i = 0; i < xycols; i++) {
		cout << setw(8*xycols) << "";
		cout << "   | ";
		for (int j = 0; j < ycols; j++) cout << setw(8) << matrixY[i][j];
		cout << endl;
	} // for

	/* print divider */
	cout << setfill('-');
	cout << setw(8*xycols+4) << "*" << setw(8*ycols+1) << "" << endl;
	cout << setfill(' ');

	/* print lower part */
	for (int i = 0; i < xrows; i++) {
		for (int j = 0; j < xycols; j++) cout << setw(8) << matrixX[i][j];
		cout << "   | ";
		for (int j = 0; j < ycols; j++) cout << setw(8) << matrixZ[i][j];
		cout << endl;
	} // for
}


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
