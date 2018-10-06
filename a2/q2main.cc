#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "PRNG.h"

using namespace std;

PRNG prng;

/* POTATO.H******************************************************************** */
class Potato {
	unsigned int tick;
	unsigned int maxTicks;
	public:
	_Event Explode {};
	Potato( unsigned int maxTicks = 10 );
	void reset( unsigned int maxTicks = 10 );
	void countdown();
};

/* POTATO.CC******************************************************************* */
Potato::Potato(unsigned int maxTicks): maxTicks(maxTicks) {
	reset();
}

void Potato::reset(unsigned int maxTicks) {
	tick = prng(1, maxTicks);
	cout << "  POTATO goes off after "
		<< tick
		<< (tick == 1 ? " toss" : " tosses")
		<< endl;
}

void Potato::countdown() {
	tick -= 1;
	if (tick == 0) {
		cout << " is eliminated" << endl;
		throw Explode();
	}
}

/* PLAYER.H******************************************************************** */
_Coroutine Player {
	int id;
	Potato *potato;
	_Event Terminate {
		public:
			Player * victim;                      // delete player
			Terminate( Player * victim ) : victim( victim ) {}
	};
	_Event Election {
		public:
			int id;                               // highest id seen so far
			Election( int id ) : id( id ) {}
	};
	Player * partner[2];                      // left and right player
	// YOU ADD MEMBERS HERE
	Player *newUmpire;
	void main(); 
	public:
	static Player * umpire;                   // current umpire

	Player( int id, Potato & potato );
	void start( Player & lp, Player & rp );   // supply partners
	void toss();                              // tossed the potato
};

/* PLAYER.CC******************************************************************* */
Player::Player(int id, Potato &potato): id( id ), potato( &potato ) {};

void Player::start(Player &lp, Player &rp) {
	partner[0] = &lp;
	partner[1] = &rp;
}

void Player::toss() {
	resume();
}

void Player::main() {
	while (true) {
		try {
			_Enable {}
		} catch (Terminate &e) {
			Player *victim = e.victim;
			Player *lp = victim->partner[0];
			Player *rp = victim->partner[1];
			lp->partner[1] = rp;
			rp->partner[0] = lp;

			potato->reset();
			cout << "U ";
			continue;
		} catch (Election &e) {
			if (Player::umpire->id == id) {
				cout << " : umpire " << newUmpire->id << endl;
				Player::umpire = newUmpire;
				_Resume Terminate(this) _At *newUmpire;
				newUmpire->toss();
				return;
			} else {
				cout << " -> " << id;
				if (e.id > id) {
					_Resume Election(e.id) _At *partner[1];
				} else {
					Player::umpire->newUmpire = this;
					_Resume Election(id) _At *partner[1];
				}
				partner[1]->toss();
				continue;
			}
		}

		if (partner[0] == this) {
			cout << id << " wins the Match!" << endl;
			return;
		}

		cout << id;
		try {
			potato->countdown();
		} catch (Potato::Explode &e) {
			if (id == Player::umpire->id) {
				cout << "E " << id;
				_Resume Election(-1) _At *partner[1];
				partner[1]->toss();
				continue;
			} else {
				_Resume Terminate(this) _At *umpire;
				umpire->toss();
				return;
			}
		}
		Player *next = partner[prng(1)];
		cout << " -> ";
		next->toss();
	}
}
/* MAIN.CC********************************************************************* */

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

	switch ( argc ) { // FIXME: d means random
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
		default:						// wrong number of options
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
		}

		int randomIndex = prng(playerNum - 1);
		swap(players[0], players[randomIndex]);		// swap random index with 0

		/* pair players */
		for (int j = 0; j < playerNum; j++) {
			players[j]->start(*players[(j-1+playerNum) % playerNum], *players[(j+1) % playerNum]);
		}

		Player::umpire = players[randomIndex];
		cout << "U ";
		Player::umpire->toss();

		/* clean up players */
		for (int j = 0; j < playerNum; j++) {
			delete players[j];
		}
		if (i != gameNum - 1) cout << endl << endl;
	}

	return 0;
} // main
