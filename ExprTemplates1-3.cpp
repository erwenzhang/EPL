#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm> // for std::min, and std::plus 

using std::cout;
using std::endl;

#define valarray valarray3

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

int main(void) {
	/* what's with std::plus? */
	std::plus<int> add_op; // old school, C++-11 and earlier
	cout << add_op(5, 7) << endl;
	cout << add_op(3.5, 2) << endl; // forces conversion from double to int

	std::plus<void> gen_add;
	cout << gen_add(5, 7) << endl;
	cout << gen_add(3.5, 2) << endl;
	cout << gen_add(2, 3.5) << endl;
}

