#include <iostream>
#include <cstdint>

using std::cout;
using std::endl;

struct Blah {};
struct Blargh {};

template <typename T>
struct Wrap : public T {
	using T::T;
	Wrap(T const& arg) : T(arg) {}
	using magic = int;
};

template <> struct Wrap<Blah> : Blah {
	using Blah::Blah;
	Wrap(Blah const& arg) : Blah(arg) {}
};

template <typename T>
void foo(T const&) {
	cout << "general\n";
}

template <typename T>
void foo(Wrap<T> const&, typename Wrap<T>::magic v = 0) {
	cout << "specific to wrapped stuff\n";
}

void foo(int const&) {
	cout << "specific to int\n";
}

template <typename T>
Wrap<T> wrap(T const& x) {
	return Wrap<T>(x);
}

void case1(void) {
	foo(42);
	Blah b;
	Blargh argh;
	foo(b);
	foo(wrap(b));

	cout << "and again\n";
	foo(argh);
	foo(wrap(argh));
}




template <typename T, typename... Args>
void bracket(T const&, Args...) {
	cout << "general\n";
}


template <typename T, typename X = decltype(std::declval<T>()[0])>
void bracket(T const& x) {
	cout << "specific to types with op[]\n";
}

template <bool p, typename T>
struct enable_if {
	using type = T;
};


template <typename T>
struct like_it {
	static constexpr bool value = false; 
};

template <> struct like_it<int> { static constexpr bool value = true; };
template <> struct like_it<float> : public std::true_type {};
template <> struct like_it<double> : public std::true_type {};

template <bool p, typename T>
using EnableIf = typename enable_if<p, T>::type;

template <typename T>
EnableIf<!like_it<T>::value, T> fred(T const& x) { cout << "general\n";  return x; }


/* use only if T::foo exists as a non-static method with zero arguments */
template <typename T>
EnableIf<like_it<T>::value, T> fred(T const& x) { cout << "second!\n";  return x; }

int main(void) {

	bracket(42);
	int x[10];
	bracket(x);
}
