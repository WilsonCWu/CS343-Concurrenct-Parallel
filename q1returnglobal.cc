#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
using namespace std;
#include <unistd.h>                                     // access: getpid

int err = 0;
short int er1;
int er2;
long int er3;

int eperiod = 10000;                                    // error period

double rtn1( double i ) {
	if ( rand() % eperiod == 0 ) {
		err = 1;
		er1 = (short int)rand();
		return 0;
	} // if
	return i;
}
double rtn2( double i  ) {
	if ( rand() % eperiod == 0 ) {
		err = 2;
		er2 = rand();
		return 0;
	} // if
	return rtn1( i ) + i;
}
double rtn3( double i  ) {
	if ( rand() % eperiod == 0 ) {
		err = 3;
		er3 = rand();
		return 0;
	} // if
	return rtn2( i ) + i;
}
int main( int argc, char * argv[] ) {
	int times = 100000000, seed = getpid();             // default values
	try {
		switch ( argc ) {
			case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
						seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
					} // if
			case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
						eperiod = stoi( argv[2] ); if ( eperiod <= 0 ) throw 1;
					} // if
			case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
						times = stoi( argv[1] ); if ( times <= 0 ) throw 1;
					} // if
			case 1: break;                                // use all defaults
			default: throw 1;
		} // switch
	} catch( ... ) {
		cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]" << endl;
		exit( EXIT_FAILURE );
	} // try
	srand( seed );

	double rv = 0.0;
	int ev1 = 0, ev2 = 0, ev3 = 0;
	int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

	for ( int i = 0; i < times; i += 1 ) {
		err = 0;
		int temp = rtn3( i );
		switch ( err ) {
			case 3:
				ev3 += er3;
				ec3 += 1;
				break;
			case 2:
				ev2 += er2;
				ec2 += 1;
				break;
			case 1:
				ev1 += er1;
				ec1 += 1;
				break;
			default:
				rv += temp;
				rc += 1;
		}
	} // for
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
