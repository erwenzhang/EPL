#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm> // for std::min

using std::cout;
using std::endl;

#define valarray valarray1

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
		Same& lhs{ *this };
		return lhs = lhs + rhs; // OK, I'm disgusted by this return statement
		/* but hey, if some dork thinks it's a good idea that operator= has a return value
		 * and I'm forced to create a return value because of that dork, well, then I'm 
		 * going to support that convention with dorky code ;-P   
		 */
	}

	Same& operator-=(Same const& rhs) {
		Same& lhs{ *this };
		return lhs = lhs - rhs;
	}
};

template <typename T>
valarray<T> operator+(valarray<T> const& lhs, valarray<T> const& rhs) {
	uint64_t size = std::min(lhs.size(), rhs.size());
	valarray<T> result(size); // need () syntax, since the result{size} looks like an initializer list

	for (uint64_t k = 0; k < size; k += 1) {
		result[k] = lhs[k] + rhs[k];
	}

	return result;
}

template <typename T>
valarray<T> operator-(valarray<T> const& lhs, valarray<T> const& rhs) {
	uint64_t size = std::min(lhs.size(), rhs.size());
	valarray<T> result(size);

	for (uint64_t k = 0; k < size; k += 1) {
		result[k] = lhs[k] - rhs[k];
	}

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

int main1(void) {
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
 * 1) OK, the += operator is horribly innefficient. Note that by dispatching to the 
 * operator+ function we have to create a temporary (the local variable "result" in
 * the operator+ function). That temporary is contructed, assigned to, returned, and then
 * stupidly read from during the assignment operator.
 *   Construction of result: allocate space potentially assign all elements to zero
 *   Assignment to result: perform N adds, 2N reads and N writes
 *   Returning result and binding result to operator= rhs: free (returned by reference, passed by reference)
 *   operator=: perform N reads and N writes to copy lhs[k] = rhs[k] 
 *   Destructor for result (performed after op= returns): deallocate memory
 * We could trivially have written the += function to perform only 2N reads and N writes
 * with no allocations
 *
 * The duplicated code between operator+ and operator- is annoying. In fact, I had an error 
 * in my first version where I declared result using the {} syntax:
 *     valarray<T> result{size};
 * which doesn't work when T is an int, because {size} looks like a initializer_list<int>
 * because I'd duplicated the code, I had to fix this same stupid bug twice.
 */

