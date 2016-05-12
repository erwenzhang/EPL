#include <iostream>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;

class SmartString : public std::string {
public:
	using std::string::string;

	void print(std::ostream& out) const {
		out << (std::string const&) (*this);
	}
};

template <typename T> struct choose_ref {
	using type = T;
};

//template<> struct choose_ref<SmartString> {
//	using type = SmartString const&;
//};

template <typename T> using ChooseRef = typename choose_ref<T>::type;

template <typename S1Type, typename S2Type>
class ConcatString {
	using LeftType = ChooseRef<S1Type>;
	using RightType = ChooseRef<S2Type>;
	LeftType s1;
	RightType s2;
public:
	ConcatString(S1Type const& left, S2Type const& right) :
		s1{ left }, s2(right) {}

	uint64_t size(void) const { return s1.size() + s2.size(); }

	char operator[](uint64_t k) const {
		if (k < s1.size()) { return s1[k]; }
		else { return s2[k - s1.size()]; }
	}

	void print(std::ostream& out) const {
		s1.print(out);
		s2.print(out);
	}
};

template <typename S>
struct Wrap : public S {
	using S::S;
	Wrap(const S& s) : S(s) {}
};

using string = Wrap<SmartString>;

template<typename S1, typename S2>
Wrap<ConcatString<S1,S2>>
operator+(Wrap<S1> const& lhs, Wrap<S2> const& rhs) {
	S1 const& left{ lhs };
	S2 const& right{ rhs };
	ConcatString<S1, S2> result{ left, right };

	Wrap<ConcatString<S1, S2>> wrapped_result{ result };

	return wrapped_result;
}


class CStringWrapper {
	const char* ptr;
public:
	CStringWrapper(const char* ptr) { this->ptr = ptr; }
	char operator[](uint64_t k) const { return ptr[k]; }
	uint64_t size(void) const { return strlen(ptr); }
	void print(std::ostream& out) const {
		out << ptr;
	}
};

template <typename S>
Wrap<ConcatString<CStringWrapper, S>>
operator+(const char* left, Wrap<S> const& right) {
	return Wrap<ConcatString<CStringWrapper, S>>{CStringWrapper{ left }, right};
}

template <typename S>
Wrap<ConcatString<S, CStringWrapper>>
operator+(Wrap<S> const& left, const char* right) {
	return Wrap<ConcatString<S, CStringWrapper>>{left, CStringWrapper{ right }};
}

template <typename T>
std::ostream& operator<<(std::ostream& out, Wrap<T> const& str) {
	str.print(out);
	return out;
}


/* --- client code here --- */

int main(void) {
	string first = "Craig";
	string last = "Chase";
	string greeting = "Hello ";
	cout << ((greeting + first) + last) << endl;
	cout << ("Hello " + first) + " " + last << endl;
}
