#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
using namespace std;
#include <unistd.h>                                     // access: getpid

typedef struct _Er1 { short int code; } Er1;
typedef struct _Er2 { int code; } Er2;
typedef struct _Er3 { long int code; } Er3;

// typedef struct name {} ActualName
typedef union _ReturnValue {
	double ret;
	Er1 er1;
	Er2 er2;
	Er3 er3;
} ReturnValue;

typedef struct _ReturnObject {
	int type; // 0: normal, X: ErX
	ReturnValue rv;
} ReturnObject;

int eperiod = 10000;                                    // error period

ReturnObject rtn1( double i ) {
    if ( rand() % eperiod == 0 ) {
		Er1 er1 = { (short int)rand() };
		ReturnValue rv;
		rv.er1 = er1;
		ReturnObject ro = { 1, rv };
		return ro;
	}
    return { .type = 0, .rv = { .ret = i } };
}
ReturnObject rtn2( double i  ) {
    if ( rand() % eperiod == 0 ) {
		Er2 er2 = { rand() };
		ReturnValue rv;
		rv.er2 = er2;
		ReturnObject ro = { 2, rv };
		return ro;
	}
	ReturnObject ret = rtn1( i );
	if (ret.type) return ret;
    return { .type = 0, .rv = { .ret = ret.rv.ret + i } };
}
ReturnObject rtn3( double i  ) {
    if ( rand() % eperiod == 0 ) {
		Er3 er3 = { rand() };
		ReturnValue rv;
		rv.er3 = er3;
		ReturnObject ro = { 3, rv };
		return ro;
	}
	ReturnObject ret = rtn2( i );
	if (ret.type) return ret;
    return { .type = 0, .rv = { .ret = ret.rv.ret + i } };
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
		ReturnObject ret = rtn3( i );
		switch ( ret.type ) {
			case 3:
				ev3 += ret.rv.er3.code;
				ec3 += 1;
				break;
			case 2:
				ev2 += ret.rv.er2.code;
				ec2 += 1;
				break;
			case 1:
				ev1 += ret.rv.er1.code;
				ec1 += 1;
				break;
			default:
				rv += ret.rv.ret;
				rc += 1;
		}
    } // for
    cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
    cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
