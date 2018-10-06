#include "q2player.h"
#include <iostream>

using std::cout;
using std::endl;

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
		} catch (Terminate &e) { // umpire catch Terminate
			Player *victim = e.victim;
			Player *lp = victim->partner[0];
			Player *rp = victim->partner[1];

			/* re-cycle */
			lp->partner[1] = rp;
			rp->partner[0] = lp;

			potato->reset();
			cout << "U ";
			continue;
		} catch (Election &e) {
			if (Player::umpire->id == id) {	// umpire catch Election
				cout << " : umpire " << newUmpire->id << endl;
				Player::umpire = newUmpire;
				_Resume Terminate(this) _At *newUmpire;
				newUmpire->toss();
				return;
			} else {						// players catch Election
				cout << " -> " << id;
				if (e.id > id) {			// id is smaller than current election id
					_Resume Election(e.id) _At *partner[1];
				} else {					// elected
					Player::umpire->newUmpire = this;
					_Resume Election(id) _At *partner[1];
				}
				partner[1]->toss();
				continue;
			} // if
		} // try

		if (partner[0] == this) {			// self-cycle
			cout << id << " wins the Match!" << endl;
			return;
		} // if

		cout << id;
		try {
			potato->countdown();
		} catch (Potato::Explode &e) {
			cout << " is eliminated" << endl;
			if (id == Player::umpire->id) {	// umpire explode
				cout << "E " << id;
				_Resume Election(-1) _At *partner[1];
				partner[1]->toss();
				continue;
			} else {						// player explode
				_Resume Terminate(this) _At *umpire;
				umpire->toss();
				return;
			} // if
		} // try
		Player *next = partner[prng(1)];
		cout << " -> ";
		next->toss();
	} // while
}
