#ifndef __OW_STACK_HPP__
#define __OW_STACK_HPP__

#include "OW_config.h"
#include "OW_Types.h"
#include "OW_Array.hpp"

template<class T>
class OW_Stack : public OW_Array<T>
{
public:
	OW_Stack() : OW_Array<T>() {  }

	bool empty() {   return (size() == 0); }

	T peek() {   return operator[](size()-1); }

	T pop() {   T obj = peek(); remove(size()-1); return obj; }

	T push(const T& x) {   push_back(x); return x; }

	int search(const T& x)
	{
		int i = find(x);
		return (i >= 0) ? int(size()) - i : -1;
	}
};

#endif	// __OW_STACK_HPP__

