#include <iostream>
#include <cstdint>
#include <typeinfo>

using std::cout;
using std::endl;

template <typename T>
auto begin(T x) -> decltype(x.begin()) {
	return x.begin();
}

template <int num, typename T>
T* begin(T array[num]) {
	return &array[0];
}

class less {
public:
	template <typename T>
	bool operator()(T const& x, T const& y) {
		return x < y;
	}
};

template <typename T>
struct Vector {
	T* pointer;
	uint64_t length;

	Vector(void) {
		pointer = nullptr;
		length = 0;
	}

	void push_back(T const& x) {}
	void push_back(T&& x) {}

	template<typename... Args>
	void emplace_back(Args&&... args) {
//		new (pointer + length) 
			T{ std::forward<Args>(args)... };
	}

	template <typename It>
	Vector(It begin, It end) : Vector() {
		while (begin != end) {
			this->push_back(*begin);
			++begin;
		}	
	}

	Vector(std::initializer_list<int> list) : Vector(list.begin(), list.end()) {}
};

struct Foo {
	Foo(void) { cout << "constructor0\n"; }
	Foo(int, const char*, double) { cout << "constructor1\n"; }
	Foo(double, int) { cout << "constructor2\n"; }
};

//template <typename T>
//void doit(T) {
//	cout << "T is " << typeid(T).name() << endl;
//	cout << "one\n";
//}

template <typename T>
struct is_ref {
	static constexpr bool value = false;

};

template <typename T>
struct is_ref<T&> {
	static constexpr bool value = true;
};

template <typename T>
void doit(T&&) {
	cout << "T is " << typeid(T).name();
	if (is_ref<T>::value) {
		cout << "&";
	}
	cout << endl;
}

int main(void) {
	doit(42);
	int x = 42;
	doit(x);
}


void fred(void) {
	less p;
	int array[10] = { 2, 4, 6, 8, 10, 12 };
	char s[] = "Hello World";
	Vector<int> vector(s, s + 12);

	vector.emplace_back();
	vector.emplace_back(42);

	Vector<Foo> vec3{};
	vec3.emplace_back();
	vec3.emplace_back(42, "hello", 3.14159);



	Vector<int> vec2 = { 1, 2, 3, 4, 5, 6 };

	if (p(5, 10)) {
		cout << "hello!";
	}
	 
	if (p(2.5, 3.0)) {
		cout << " world\n";
	}

	int x[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int n = 42;
}
