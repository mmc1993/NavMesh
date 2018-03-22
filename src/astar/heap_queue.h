#pragma once

#include "../base.h"

template <class T>
class HeapQueue : public std::priority_queue<T> {
public:
	typename std::priority_queue<T>::container_type::const_iterator cbegin() const
	{
		return c.cbegin();
	}

	typename std::priority_queue<T>::container_type::const_iterator cend() const
	{
		return c.cend();
	}

	void clear()
	{
		c.clear();
	}
private:

};