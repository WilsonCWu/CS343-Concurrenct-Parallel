template<typename T> _Coroutine Binsertsort {
	T value;                        // communication: value being passed down/up the tree

	void getChildData(Binsertsort<T> &b);	// retrieve values stored in coroutine b and its children

	void main();
	public:
	_Event Sentinel {};
	void sort( T value ) {          // value to be sorted
		Binsertsort::value = value;
		resume();
	}
	T retrieve() {                  // retrieve sorted value
		resume();
		return value;
	}
};

template<typename T> void Binsertsort<T>::getChildData(Binsertsort<T> &b) {
	_Resume Sentinel {} _At b;
	try {
		_Enable {
			while (true) {
				value = b.retrieve();
				suspend();
			} // while
		} // _Enable
	} catch (Sentinel &e) {}
}

template<typename T> void Binsertsort<T>::main() {
	// get non-local exception when no item is in Binsertsort
	try { _Enable {} } catch (Sentinel &e) {
		_Resume Sentinel {} _At resumer();
		return;
	} // try

	T pivot = value;
	Binsertsort<T>less;
	Binsertsort<T>greater;

	try {
		_Enable {
			// read in values
			while (true) {
				suspend();
				if (value < pivot) less.sort(value);
				else greater.sort(value);
			} // while
		} // _Enable
	} catch (Sentinel &e) {
		getChildData(less);		// get left child item
		value = pivot;			// get pivod item
		suspend();
		getChildData(greater);	// get right child item
	} // try

	_Resume Sentinel {} _At resumer();	// indicate no more items
	return ;
}
