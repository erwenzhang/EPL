#include <iostream>
#include <cstdint>
#include <vector>
#include <list>

using std::cout;
using std::endl;

template <typename T>
void swap(T&& x, T&& y) {
	std::remove_reference<T>::type tmp{ std::move(x) };
	x = std::move(y);
	y = std::move(tmp);
}

template <typename T>
class GenComp {
public:
	bool operator()(T const& x, T const& y) {
		return x < y;
	}
};

/* 
* Goals for STL
* 1. generic with respect to element type
* 2. generic with respect to data structure type
* 3. predictable time complexity
* 4. comparable/competitive performance with hand-written custom code
*/

template <typename Iterator>
void sort(Iterator b, Iterator e) {
	using Elem = decltype(*b);
	sort(b, e, GenComp<Elem>{});
}

template <typename T, typename Comp>
void sort(T b, T e, Comp _not_used) {
	Comp fun;
	if (b == e) { return; } // done
	T count{ b };
	++count;

	while (count != e) {
		T j{ b };
		T jplus1{ j };
		++jplus1;
		while (jplus1 != e) {
			if (fun(*jplus1, *j)){
				swap(*j, *jplus1);
			}
			++j;
			++jplus1;
		}
		++b;
		++count;
	}
}

/* fundamental operators on pointers
 * 1. dereference (operator*)
 * 2. check for equality (operator==)
 * 3. advance (operator++)
 *     the above are "forward iterators"
 * 4. retreat (operator--)
 *     the above are "bidirectional iterators"
 * 5. advance by an integer amount (operator+)
 * 6. calculate a distance (operator-)
       the above are "random access iterators"

 p != q is really !(p == q)
 p < q is really p - q < 0
 p[k] is really *(p + k)
 */


template <typename T>
T max(T const& x, T const& y) {
	if (x < y) { return y;  }
	else { return x; }
}


/*
 * in STL: the convention is that a container is identified as the range
 * [b, e)
 * i.e., e is one AFTER the last element
 */
template <typename It>
void printContainer(It b, It e) {
	const char* pref = "";
	while (b != e) {
		cout << pref << *b;
		++b;
		pref = ", ";
	}
	cout << endl;
}

int main(void) {
	int x[] = { 5, 3, 8, 1, 6, 9, 2, 4, 0, 7 };
	int n = 10;
	std::list<int> y = { 5, 3, 8, 1, 6, 9, 2, 4, 0, 7 };
	std::list<int>::iterator b = y.begin();
	auto e = y.end();
	cout << "original: ";  printContainer(b, e);
	/* NOTE:
	 * the conventional name of the iterator type for any data structure
	 * (e.g., list<int>) is DS::iterator 
	 * e.g., list<int>::iterator */
	sort(y.begin(), y.end(), GenComp<int>{});
	sort(y.begin(), y.end());
	cout << "sorted: ";  printContainer(y.begin(), y.end());

	const char* pref = "";
	/* equivalent to:
	 * for (auto p = begin(y), int val = *p; p != end(y); ++p, val = *p) 
	 */
	for (int val : y) {
		cout << pref << val;
		pref = ", ";
	}
	cout << endl;

	for (const auto & val : y) {
		cout << pref << val;
		pref = ", ";
	}
	cout << endl;

	cout << max(3.5, 1.5) << endl;
}
