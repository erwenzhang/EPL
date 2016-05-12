#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm> // for std::min, and std::plus 

using std::cout;
using std::endl;

#define valarray valarray2

template <typename T>
class valarray;

template <typename Op, typename T>
void apply_op(valarray<T>& lhs, valarray<T> const& x, valarray<T> const& y, Op op = Op{}) {
	uint64_t size = std::min(x.size(), y.size());
	size = std::min(size, lhs.size()); // probably not needed
	for (uint64_t k = 0; k < size; k += 1) {
		lhs[k] = op(x[k], y[k]);
	}
}

template <typename T>
class valarray : public std::vector<T> {
	using Same = valarray<T>;
public:
	using std::vector<T>::vector;

	/* change the semantics of assignment, and provide only copy assignment (no move) */
	Same& operator=(Same const& rhs) {
		/* don't need to protect against x = x, although it might help performance to do so */

		uint64_t size = std::min(this->size(), rhs.size()); // only assign min # elems

		Same& lhs{ *this }; // a convenience variable, reference to self "left hand side"

		for (uint64_t k = 0; k < size; k += 1) {
			lhs[k] = rhs[k];
		}

		return *this;
	}

	/* note: it does not make sense to change the semantics of initialization, right??? */

	/* operator assignments, must be members */
	Same& operator+=(Same const& rhs) {
		apply_op<std::plus<>>(*this, *this, rhs);
		return *this;
	}

	Same& operator-=(Same const& rhs) {
		apply_op<std::minus<>>(*this, *this, rhs);
		return *this;
	}
};

template <typename T>
valarray<T> operator+(valarray<T> const& lhs, valarray<T> const& rhs) {
	valarray<T> result(std::min(lhs.size(), rhs.size())); 
	apply_op<std::plus<>>(result, lhs, rhs);
	return result;
}

template <typename T>
valarray<T> operator-(valarray<T> const& lhs, valarray<T> const& rhs) {
	valarray<T> result(std::min(lhs.size(), rhs.size()));
	apply_op<std::minus<>>(result, lhs, rhs);
	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const valarray<T>&  vec) {
	const char* pref = "";
	for (const auto& val : vec) {
		out << pref << val;
		pref = ", ";
	}
	return out;
}

int main2(void) {
	valarray<int> x{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	valarray<int> y{ 2, 4, 6, 8, 10 };
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "x + y: " << x + y << endl;
	x += y;
	cout << "x: " << x << endl;

	return 0;
}

/* critiques:
 * a step in the right direction, certainly. However, this short example demonstrates how
 * quickly C++ templates can result in code that does not look familiar at all.
 * For example, what the heck is going on with std::plus<> ?
 * My personal opinion is that template programming in C++ requires especially diligent
 * use of style. Part of my motivation comes from the fact that our C++ compilers provide
 * very little support to debug incorrect templates, so it's essential that we get things
 * correct. For example, I had a small bug where I thought that std::min() could accept
 * more than two arguments (why not, a variadic function makes perfect sense).
 *    uint64_t size = std::min(lhs.size(), x.size(), y.size()); // from apply_op
 *
 * Unfortunately for me, the 3-argument std::min function does exist, it just expects the third
 * argument to be a comparison function object. I provided an integer as my third argument
 * so naturally the program didn't compile. This very stupid mistake took almost 30 minutes for
 * me to fix, because the error messages flagged an issue with apply_op and I naturally
 * assumed the issue was with the way I used std::plus
 * 
 * Anyway, hopefully that motivates a detour into a quick review of std::plus
 *
 * The bigger critque remains this, the code for x = y + z is still inefficient, as
 * it creates a temporary result for y + z and then uses assignment to copy those temporary
 * values into x. That's silly, when compared to would happen if we weren't using this
 * valarray library (we'd write the code)
 *    for (uint64_t k = 0; k < size; k += 1) {
 *       x[k] = y[k] + z[k];
 *    }
 * which performs no allocations or deallocations and does only 2N reads and N writes.
 * If we follow this logic one step forward, we see that operator overloading has a serious
 * flaw -- it impedes any chance to optimize the code, combine loops, avoid allocations, utilize
 * the registers of the processor, etc. More simply consider that 
 * while we improved the case for x += y, we still don't have a case for either
 *       x += y + z;
 *       w = x + y + z;
 * or for any other more complex operation. The goal for Project 3 is to solve this problem
 * using "expression templates"
 */

