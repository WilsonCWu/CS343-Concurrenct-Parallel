#include <iostream>
#include <variant>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
using namespace std;
#include <unistd.h>                                     // access: getpid

class Er {
	int err;
	public:
	Er ( int err ): err( err ) {}
	int geterr() { return err; }
	virtual ~Er() = 0;
};

Er::~Er(){};

class Er1: public Er {
	short int code;
	public:
	Er1 ( short int code ) : Er( 1 ), code( code ) {}
	short int getCode() { return code; }
};

class Er2: public Er {
	int code;
	public:
	Er2 ( int code ) : Er( 2 ), code( code ) {}
	int getCode() { return code; }
};

class Er3: public Er {
	long int code;
	public:
	Er3 ( long int code ) : Er( 3 ), code( code ) {}
	long int getCode() { return code; }
};

int eperiod = 10000;                                    // error period

variant<double, Er *> rtn1( double i ) {
	if ( rand() % eperiod == 0 ) {
		return new Er1{ (short int)rand() };
	}
	return i;
}
variant<double, Er *> rtn2( double i  ) {
	if ( rand() % eperiod == 0 ) {
		return new Er2{ rand() };
	}
	variant<double, Er *>ret = rtn1( i );
	if ( holds_alternative<double>( ret ) ) {
		return get<double>( ret ) + i;
	}
	return get<Er *>( ret );
}
variant<double, Er *> rtn3( double i  ) {
	if ( rand() % eperiod == 0 ) {
		return new Er3{ rand() };
	}
	variant<double, Er *>ret = rtn2( i );
	if ( holds_alternative<double>( ret ) ) {
		return get<double>( ret ) + i;
	}
	return get<Er *>( ret );
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
		variant<double, Er *>ret = rtn3( i );
		if ( holds_alternative<double>( ret ) ) {
			rv += get<double>( ret );
			rc += 1;
			continue;
		}
		switch ( get<Er *>( ret )->geterr() ) {
			case 3: {
				Er3 *er3 = dynamic_cast<Er3 *>( get<Er *>( ret ) );
				ev3 += er3->getCode();
				ec3 += 1;
				break;
			}
			case 2: {
				Er2 *er2 = dynamic_cast<Er2 *>( get<Er *>( ret ) );
				ev2 += er2->getCode();
				ec2 += 1;
				break;
			}
			case 1: {
				Er1 *er1 = dynamic_cast<Er1 *>( get<Er *>( ret ) );
				ev1 += er1->getCode();
				ec1 += 1;
				break;
			}
		}
	} // for
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
