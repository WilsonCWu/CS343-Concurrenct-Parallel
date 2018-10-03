#include <string>

using std::string;

_Coroutine Listing {
	char ch; // character passed by cocaller

	bool contains(string s, char c);
	void checkEOL();
	void validRead(string s);
	void validReadBlock(string s);
	void main(); // coroutine main

	public:
	_Event Match {}; // last character match
	_Event Error {}; // last character invalid
	void next( char c ) {
		ch = c; // communicate input
		resume(); // activate
	}
};
