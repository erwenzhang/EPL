#ifndef _MKS_h
#define _MKS_h 1

template <typename T, int ME, int SE, int GE, int NUM, int DEN>
class MKS {
	T value; 
public:
	typedef MKS<T, ME,SE,GE,NUM,DEN> Same;
	static char unit_name[8]; 

	/* initialization is permitted explicity from an ordinary scalar,
	* or from an object with exactly the same units */
	explicit MKS(double x=0.0) : value(x) {}
	MKS(const Same& that) : value(that.value) {}

	~MKS(void) {} //nothing to destroy
    
    T getValue(void) const { return value; }
    
	/* assignment is only permitted with an object of the same units
	* (see type conversion below) */
	Same& operator=(const Same& that) {
		value = that.value;
		return *this;
	}

	/* addition and subtraction is permitted only with like units */
	Same operator+(const Same& that) const{ return Same(value + that.value); }
    Same operator-(const Same& that) const { return Same(value - that.value); }
    
    void operator+=(const Same& that) { value += that.value; }
	void operator-=(const Same& that) {  value -= that.value;	}
	
	Same operator-(void) const { return Same(-value); }

	/* comparisons are possible only with like units */
	bool operator==(const Same& that) const { return value == that.value; }
	bool operator<(const Same& that) const { return value < that.value; }
	bool operator>(const Same& that) const { return that < *this; }
	bool operator!=(const Same& that) const { return !(*this == that); }
	bool operator<=(const Same& that) const { return !(that < *this); }
	bool operator>=(const Same& that) const { return !(*this < that); }
    
    /* Units can be scaled, e.g., "give me twice x" */
    Same operator*(double scale) const { return Same(scale * value); }
    Same operator/(double scale) const { return Same(value / scale); }
    
	/* multiplication and division is permitted between arbitrary units,
	* but only with unit scales
	*/
	template <int ME2, int SE2, int GE2,int NUM2, int DEN2>
	MKS<T, ME+ME2,SE+SE2,GE+GE2,NUM,DEN> 
	operator*(const MKS<T, ME2,SE2,GE2,NUM2,DEN2>& that) const {
		return MKS<T, ME+ME2,SE+SE2,GE+GE2,NUM,DEN>(value * NUM2 
												* that.getValue() / DEN2);
	}
	template <int ME2, int SE2, int GE2, int NUM2, int DEN2>
	MKS<T, ME-ME2,SE-SE2,GE-GE2,NUM,DEN> 
    operator/(const MKS<T, ME2,SE2,GE2,NUM2,DEN2>& that) const {
		return MKS<T, ME-ME2,SE-SE2,GE-GE2,NUM,DEN>((value * DEN2) /
													(that.getValue() * NUM2));
	}

	template <int NUM2, int DEN2>
	operator MKS<T, ME,SE,GE,NUM2,DEN2>(void) const {
		double convertMeTo1 = (double) NUM / (double) DEN;
		double convert1ToThem = (double) DEN2 / (double) NUM2;
		return MKS<T, ME, SE, GE, NUM2, DEN2>(T{ value * convertMeTo1 * convert1ToThem });
	}
};

template <typename T, int ME, int SE, int GE, int NUM, int DEN>
MKS<T, ME,SE,GE,NUM,DEN> operator*(T x, const MKS<T, ME,SE,GE,NUM,DEN>& y) {
  return MKS<T, ME,SE,GE,NUM,DEN> (x * y.getValue());
}

template <typename T, int ME, int SE, int GE, int NUM, int DEN>
MKS<T, -ME,-SE,-GE,NUM,DEN> operator/(T x, const MKS<T, ME,SE,GE,NUM,DEN>& y) {
  return MKS<T, -ME,-SE,-GE,NUM, DEN>(x / y.getValue());
}

template<typename T, int ME, int SE, int GE, int NUM, int DEN>
std::ostream& operator<<(std::ostream& ost, const MKS<T, ME,SE,GE,NUM,DEN>& x) {
  return ost << x.getValue() << MKS<T, ME,SE,GE,NUM,DEN>::unit_name;
}

template <typename T, int ME, int SE, int GE, int NUM, int DEN>
char MKS<T, ME,SE,GE,NUM,DEN>::unit_name[] = "MKS";

template <typename T> using Meter = MKS<T, 1,0,0,1,1>;
template<> char MKS<double, 1,0,0,1,1>::unit_name[] = "m";

template <typename T> using Kilometer = MKS<T, 1,0,0,1000,1>;
template<> char MKS<double, 1,0,0,1000,1>::unit_name[] = "km";

template <typename T> using Inch = MKS<T, 1,0,0,100,3937>;
template<> char MKS<double, 1,0,0,100,3937>::unit_name[] = "in";

template <typename T> using Foot = MKS<T, 1, 0, 0, 1200, 3937>;
template<> char MKS<double, 1,0,0,1200,3937>::unit_name[] = "ft";

template <typename T> using Mile = MKS<T, 1,0,0,1200*5280,3937>;
template<> char MKS<double, 1,0,0,1200*5280,3937>::unit_name[] = "mile";

template <typename T> using Second = MKS<T, 0,1,0,1,1>;
template<> char MKS<double, 0,1,0,1,1>::unit_name[] = "s";

template <typename T> using Hour = MKS<T, 0,1,0,3600,1>;
template<> char MKS<double, 0,1,0,1,3600>::unit_name[] = "hr";

template <typename T> using Gram = MKS<T, 0,0,1,1,1>;
template<> char MKS<double, 0,0,1,1,1>::unit_name[] = "g";

template <typename T> using MeterPerSecond = MKS<T, 1,-1,0,1,1>;
template<> char MKS<double, 1,-1,0,1,1>::unit_name[] = "mps";

template <typename T> using KilometerPerSecond = MKS<T, 1,-1,0,1000,1>;
template<> char MKS<double, 1,-1,0,1000,1>::unit_name[] = "kmps";

template <typename T> using KilometerPerHour = MKS<T, 1,-1,0,1000,3600>;
template<> char MKS<double, 1,-1,0,1000,3600>::unit_name[] = "kph";

template <typename T> using MilePerHour = MKS<T, 1,-1,0,1200*5280,3927*3600>;
template<> char MKS<double, 1,-1,0,1200*5280,3927*3600>::unit_name[] = "mph";

template <typename T> using MeterPerSecond2 = MKS<T, 1,-2,0,1,1>;
template<> char MKS<double, 1, -2, 0, 1, 1>::unit_name[] = "mps2";

#endif /* _MKS_h */
