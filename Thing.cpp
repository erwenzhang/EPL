#include <iostream>
#include <cstdint>
#include <string>

using std::cout;
using std::endl;
using std::string;

class ObservableString : public std::string {
public:
	ObservableString(const char* s) : std::string{ s } {}
	ObservableString(void) {}
	ObservableString(ObservableString const& rhs) : std::string{ rhs } {
		cout << "copy of \"" << rhs << "\"\n";
	}
	~ObservableString(void) {
		cout << "string \"" << *this << "\" destroyed\n";
	}
};


class ThingHelperInterface {
public:
	virtual void print(std::ostream&) const = 0;
	virtual ~ThingHelperInterface(void) {}
};

template <typename T>
class ThingHelper : public ThingHelperInterface {
public:
	T obj;
	ThingHelper(T const& arg) : obj{ arg } {}
	virtual void print(std::ostream& out) const {
		out << obj;
	}
};

class Thing {
	ThingHelperInterface* obj = nullptr;
public:


	template <typename T>
	Thing& operator=(T const& rhs) {
		delete obj;
		obj = new ThingHelper<T>(rhs);
		return *this;
	}

	void print(std::ostream& out) const {
		obj->print(out);
	}

	~Thing(void) {
		delete obj;
	}

	template <typename T>
	operator T(void) const {
		ThingHelper<T>* cast_obj = dynamic_cast<ThingHelper<T>*>(obj);
		if (cast_obj) {
			return cast_obj->obj;
		} else {
			return T{};
		}
	}
};


std::ostream& operator<<(std::ostream& out, Thing const& thing) {
	thing.print(out);
	return out;
}

int main(void) {
	Thing x;
	x = 42;
	cout << x << endl; // prints "42"

	cout << (int)x << endl;

	//x = "hello world";
	//cout << x << endl; // prints "hello world";

	x = ObservableString{ "Hello World" };
	cout << "OK, we've assigned a string now...\n";
	cout << x << endl; // prints "Hello World";

	cout << (int)x << endl;
	cout << (ObservableString)x << endl;


	/* independent of whether this last assignment statement is performed,
	 * when x goes out of scope, all appropriate destructors are run 
	 * (e.g., the string is correctly destructed) */
//	x = 42;
} 
