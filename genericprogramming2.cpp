#include <iostream>
#include <cstdint>
#include <vector>
#include <list>
#include <functional>
#include <typeinfo>
#include <string>

#include "Generic.h"

using std::cout;
using std::endl;

/* a generic state-less function object for using default comparisons */
template <typename T>
struct less {
	bool operator()(T const& x, T const& y) const {
		return x < y;
	}
};

template <typename T>
void showSort(T& container) {
	cout << "test for " << typeid(T).name() << endl;
	cout << "\tprior to sorting: ";
	const char* pref = "";
	for (auto const & val : container) {
		cout << pref << val;
		pref = ", ";
	}
	cout << endl;

	auto b = std::begin(container);
	auto e = std::end(container);
	using Elem = decltype(*b);

	quickSort(b, e, less<Elem>{});

	cout << "\t   after sorting: ";
	pref = "";
	for (auto const & val : container) {
		cout << pref << val;
		pref = ", ";
	}
	cout << endl;
	cout << endl;
}


int main(void) {
	int x[] = { 5, 3, 9, 2, 1, 0, 4, 8, 7, 6 };
	std::vector<int> y{ std::begin(x), std::end(x) };
	std::list<int> z{ std::begin(x), std::end(x) };

	showSort(x);
	showSort(y);
	showSort(z);

	/* not ready for this one yet... we need some type-deduction logic 
	 * The problem is that our sorting functions know the iterator type we have
	 * (e.g., std::vector<int>::iterator)
	 * but the sorting functions don't know what type is stored in the container
	 * I know, it seems silly that std::vector<int>::iterator doesn't give us 
	 * enough info to realize that "int" is the type of data we're sorting, but we don't 
	 * have that info... we need some tools -- Thursday's class */
	std::vector<std::string> words{ "cat", "dog", "bird", "mouse", "elephant" };
	showSort(words);
}
