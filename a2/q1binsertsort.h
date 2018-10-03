template<typename T> _Coroutine Binsertsort {
	T value;                        // communication: value being passed down/up the tree

	void getChildData(Binsertsort<T> &b) {
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

	void main() {
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
					else greater.sort(value); // TODO: test this, equal case
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
	};
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
