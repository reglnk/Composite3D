#include <cm3d/Tl/Queue.hpp>

#include <iostream>
#include <cassert>

using namespace cm3d;

int main()
{
	Queue<int> q;
	int num;
	for (int i = 100000; i; --i) {
		num = i << (i & 32);
		num >>= i & 12;
		num *= i & 279;
		q.push(num);
		assert(q.back() == num);
	}
	for (int i = 100000; i; --i) {
		int rnum = i << (i & 32);
		rnum >>= i & 12;
		rnum *= i & 279;
		assert(rnum == q.front());
		assert(num == q.back());
		q.pop();
	}
	assert(q.length() == 0);
	return 0;
}
