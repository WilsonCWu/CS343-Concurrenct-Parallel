#include <iostream>
using namespace std;

volatile int iterations = 10000000, shared = 0; // volatile to prevent dead-code removal

_Task increment {
	void main() {
		for ( int i = 1; i <= iterations; i += 1 ) {
			shared += 1;                        // two increments to increase pipeline size
			shared += 1;
		}
	}
};
int main( int argc, char * argv[] ) {
	try {                                       // process command-line arguments
		switch ( argc ) {
			case 2: iterations = stoi( argv[1] ); if ( iterations <= 0 ) throw 1;
			case 1: break;                      // use defaults
			default: throw 1;
		} // switch
	} catch( ... ) {
		cout << "Usage: " << argv[0] << " [ iterations (> 0) ]" << endl;
		exit( 1 );
	} // try
#ifdef __U_MULTI__
	uProcessor p;                              // create 2nd kernel thread
#endif // __U_MULTI__
	{
		increment t[2];
	} // wait for tasks to finish
	cout << "shared:" << shared << endl;
}
