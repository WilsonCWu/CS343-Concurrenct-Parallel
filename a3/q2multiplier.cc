#include "q2multiplier.h"

// shared variables
int Multiplier::xr;
int Multiplier::xc;
int Multiplier::yc;
int **Multiplier::X;
int **Multiplier::Y;
int **Multiplier::Z;

void Multiplier::main() {
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
