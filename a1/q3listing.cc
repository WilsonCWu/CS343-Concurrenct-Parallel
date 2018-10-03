#include <iostream>
#include <string>
#include <locale>
#include "q3listing.h"

using namespace std;
#define ALPHA "abcdefghigklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ"
#define DIGIT "0123456789"
#define FILE_INFO "dDbcps-"
#define READ "r-"
#define WRITE "w-"
#define EXECUTE "x-"
#define WHITESPACE " \t"

/*
 * return if c appears in s
 */
bool Listing::contains(string s, char c) {
	return s.find(c) != string::npos;
}

/*
 * Check if End of Line reached
 */
void Listing::checkEOL() {
	if (ch == '\n') {
		_Resume Error {} _At resumer();
		suspend();
	} // if
}

void Listing::validRead(string s) {
	checkEOL();
	cout << ch;
	if (!contains(s, ch)) {
		_Resume Error {} _At resumer();
	} // if
	suspend();
}

void Listing::validReadBlock(string s) {
	while (true) {
		if (!contains(s, ch)) break;
		cout << ch;
		suspend();
	} // while
}

void Listing::main() {
	validRead(FILE_INFO);			// read file info

	for (int i = 0; i < 3; i++) {	// read permissions
		validRead(READ);
		validRead(WRITE);
		validRead(EXECUTE);
	} // for

	validRead(WHITESPACE);			// check at least one whitespace
	validReadBlock(WHITESPACE);		// read in rest of whitespaces

	if (ch == '0') {				// read in file size
		cout << ch;
		suspend();
	} else if (ch >= '1' && ch <= '9') {
		validReadBlock(DIGIT);
	} else {						// leading zero not allowed
		cout << ch;
		_Resume Error {} _At resumer();
		suspend();
	} // if

	validRead(WHITESPACE);			// check at least one whitespace
	validReadBlock(WHITESPACE);		// read in rest of whitespaces

	validRead(ALPHA "_.");			// check file start with legal char
	validReadBlock(ALPHA DIGIT "_.");// read in rest of file name


	if (ch == '*' || ch == '/') {	// check file name end with * or /
		cout << ch;
		suspend();
	}

	if (ch == '\n') {				// if end of line, then matched
		_Resume Match {} _At resumer();
		suspend();
	} else {						// extra characters
		cout << ch;
		_Resume Error {} _At resumer();
		suspend();
	} // if
}

