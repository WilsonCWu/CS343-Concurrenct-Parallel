#ifndef __BOUNDEDBUFFER_H__
#define __BOUNDEDBUFFER_H__

#include <queue>
#include <cassert>

using namespace std;

template<typename T> class BoundedBuffer {
	uOwnerLock lock;
	uCondLock consLock, prodsLock;
	int volatile consSignal = 0, prodsSignal = 0;

	unsigned int size;
	queue<T>items;
	public:
	BoundedBuffer( const unsigned int size = 10 );
	void insert( T elem );
	T remove();
};

template<typename T>
BoundedBuffer<T>::BoundedBuffer(const unsigned int size): size(size) {}; 

template<typename T>
void BoundedBuffer<T>::insert(T elem) {
	lock.acquire();

#ifdef BUSY                            // busy waiting implementation
	while (items.size() == size) {
		prodsLock.wait(lock);
	} // while
#endif // BUSY
#ifdef NOBUSY                          // no busy waiting implementation
	if (prodsSignal > 0 || items.size() == size) {
		prodsLock.wait(lock);
		prodsSignal -= 1;
	} // if
#endif // NOBUSY

	assert(items.size() < size);
	items.push(elem);

#ifdef NOBUSY                          // no busy waiting implementation
	if (prodsSignal == 0 && !prodsLock.empty() && items.size() < size) {
		prodsSignal += 1;
		prodsLock.signal();
	} // if
	if (!consLock.empty()) {
		consSignal += 1;
	} // if
#endif // NOBUSY

	consLock.signal();
	lock.release();
} // insert()

template<typename T>
T BoundedBuffer<T>::remove() {
	lock.acquire();

#ifdef BUSY                            // busy waiting implementation
	while (items.size() == 0) {
		consLock.wait(lock);
	} // while
#endif // BUSY
#ifdef NOBUSY                          // no busy waiting implementation
	if (consSignal > 0 || items.size() == 0) {
		consLock.wait(lock);
		consSignal -= 1;
	} // if
#endif // NOBUSY

	assert(items.size() > 0);
	T elem = items.front();
	items.pop();

#ifdef NOBUSY                          // no busy waiting implementation
	if (consSignal == 0 && !consLock.empty() && items.size() > 0) {
		consSignal += 1;
		consLock.signal();
	} // if
	if (!prodsLock.empty()) {
		prodsSignal += 1;
	} // if
#endif // NOBUSY

	prodsLock.signal();
	lock.release();

	return elem;
} // remove()

#endif // __BOUNDEDBUFFER_H__
