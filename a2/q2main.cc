#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "PRNG.h"
#include "q2potato.h"
#include "q2player.h"

using namespace std;

PRNG prng;

bool convert( int &val, char *buffer ) {		// convert C string to integer
	std::stringstream ss( buffer );			// connect stream and buffer
	string temp;
	ss >> dec >> val;					// convert integer from buffer
	return ! ss.fail() &&				// conversion successful ?
		! ( ss >> temp );				// characters after conversion all blank ?
} // convert

void usage( char *argv[] ) {
	cerr << "Usage: " << argv[0]
		<< " [ games | 'd' [ players | 'd' [ seed | 'd' ]]]" << endl;
	exit( EXIT_FAILURE );				// TERMINATE
} // usage

Player *Player::umpire;

int main(int argc, char *argv[]) {
	int gameNum = 5, playerNum, seed = getpid();

	switch ( argc ) {
		case 4:
			if (strcmp(argv[3], "d") != 0 &&
					(! convert( seed, argv[3] ) || seed <= 0 )) {			// invalid integer ?
				usage( argv );
			} // if
			// FALL THROUGH
		case 3:
			if (strcmp(argv[2], "d") != 0 &&
					(! convert( playerNum, argv[2] ) || playerNum < 2 )) {	// invalid integer ?
				usage( argv );
			} // if
			// FALL THROUGH
		case 2:
			if (strcmp(argv[1], "d") != 0 &&
					(! convert( gameNum, argv[1] ) || gameNum < 0 )) {		// invalid integer ?
				usage( argv );
			} // if
			// FALL THROUGH
		case 1:						// all defaults
			break;
		default:					// wrong number of options
			usage( argv );
	} // switch

	prng.seed(seed);
	for (int i = 0; i < gameNum; i++) {
		/* set player number */
		if (argc < 3 || strcmp(argv[2], "d") == 0) playerNum = prng(2, 10);

		cout << playerNum << " players in the game" << endl;

		Potato potato;
		Player *players[playerNum];

		/* initial players */
		for (int j = 0; j < playerNum; j++) {
			players[j] = new Player(j, potato);
		} // for

		int randomIndex = prng(playerNum - 1);
		swap(players[0], players[randomIndex]);		// swap random index with 0

		/* pair players */
		for (int j = 0; j < playerNum; j++) {
			players[j]->start(*players[(j-1+playerNum) % playerNum], *players[(j+1) % playerNum]);
		} // for

		Player::umpire = players[randomIndex];		// choose first umpire

		/* start toss */
		cout << "U ";
		Player::umpire->toss();

		/* clean up players */
		for (int j = 0; j < playerNum; j++) {
			delete players[j];
		} // for

		if (i != gameNum - 1) cout << endl << endl;
	}

	return 0;
} // main
