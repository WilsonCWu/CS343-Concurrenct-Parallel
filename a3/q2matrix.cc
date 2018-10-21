#include "q2multiplier.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <uCobegin.h>

using namespace std;

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

void readMatrix(ifstream *fin, int *matrix[], unsigned int row, unsigned int col) {
	for (unsigned int i = 0; i < row; i++) {
		matrix[i] = new int [col];
		for (unsigned int j = 0; j < col; j++) *fin >> matrix[i][j];
	} // while
}

void createMatrix(int *matrix[], unsigned int row, unsigned int col) {
	while (row -- > 0) {
		matrix[row] = new int [col];
		for (unsigned int i = 0; i < col; i++) matrix[row][i] = 37;
	} // while
}

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


