#ifndef __OW_STACK_HPP__
#define __OW_STACK_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Array.hpp"

template<class T>
class OW_Stack : private OW_Array<T>
{
public:

	typedef typename OW_Array<T>::size_type size_type;
	typedef typename OW_Array<T>::reference reference;
	typedef typename OW_Array<T>::const_reference const_reference;

	OW_Stack() : OW_Array<T>() {  }

	bool empty() const {   return (size() == 0); }

	reference top() {   return operator[](size()-1); }
	const_reference top() const {   return operator[](size()-1); }

	void pop() { remove(size()-1); }

	void push(const T& x) {   push_back(x); }

	int search(const T& x) const
	{
		int i = find(x);
		return (i >= 0) ? int(size()) - i : -1;
	}

	size_type size() const
	{
		return OW_Array<T>::size();
	}
};

#endif	// __OW_STACK_HPP__

