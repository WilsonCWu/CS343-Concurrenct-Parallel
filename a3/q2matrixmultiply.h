#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <fstream>

using namespace std;

void matrixmultiply(int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc);

/*
 * read matrix from file
 */
void readMatrix(ifstream *fin, int *matrix[], unsigned int row, unsigned int col);


/*
 * create matrix with default entry value 37
 */
void createMatrix(int *matrix[], unsigned int row, unsigned int col);

/*
 * print matrix
 */
void printMatrix(int *matrixX[], int *matrixY[], int *matrixZ[], int xrows, int xycols, int ycols);

#endif
