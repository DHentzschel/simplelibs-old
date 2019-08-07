#ifndef ASYNCAQUEUE_H
#define ASYNCAQUEUE_H

#include "aqueue.h"
#include "alist.h"

#include <mutex>

template <class T>
class AsyncAQueue : protected AQueue<T> {
public:
	void enqueue(const T& value) override;

	T dequeue() override;

	bool isEmpty() override;

private:
	std::mutex mutex_;

};

template<class T>
inline void AsyncAQueue<T>::enqueue(const T& value)
{
	mutex_.lock();
	AList<T>::append(value);
	mutex_.unlock();
}

template<class T>
inline T AsyncAQueue<T>::dequeue()
{
	mutex_.lock();
	auto result = AList<T>::takeFirst();
	mutex_.unlock();
	return result;
}

template<class T>
inline bool AsyncAQueue<T>::isEmpty()
{
	mutex_.lock();
	auto result = AQueue<T>::isEmpty();
	mutex_.unlock();
	return result;
}

#endif // !ASYNCAQUEUE_H
