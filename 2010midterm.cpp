#include<iostream>
#include <typeinfo>
#include<vector>
#include<list>
#define prob14


#ifdef prob11//2010 midterm 1
using std::cout;
template<typename It>
void reverse(It b,It e){
	e = e - 1;
	while(b<e){
		auto tmp = *b;
		*b=*e;
		*e=tmp;
		++b;
		--e;
	}
}
int main(void){
	int x[5] ={1,2,3,4,5};
	reverse(std::begin(x),std::end(x));
	cout<<x[0]<<x[1]<<x[2]<<x[3]<<x[4];

	getchar();

}





#ifdef prob12 //2010 midterm 2
template<int size>
struct compute{
	static const int value =2*compute<size/2>::value;
};

template<>
struct compute<0>{
	static const int value = 1;
};

int main(void){

	int x;
    const int val = compute<((sizeof(x)-1)>0?(sizeof(x)-1):sizeof(x))>::value;
	int y[val];
	std::cout<<val;

	getchar();
}

#endif


#ifdef prob13 //2010 midterm 5

template<typename T>
bool isRandom(T p){
	return IsRandom( typename std::iterator_traits<T>::iterator_category{});
}
template<typename It_tag>
bool IsRandom(It_tag){
	return false;
}

bool IsRandom(std::random_access_iterator_tag){
	return true;
}

int main(void){

	std::vector<int> x;
	std::list<int> y;
	std::cout<<isRandom(x.begin());
	std::cout<<isRandom(y.begin());
	getchar();
}

#endif

#ifdef prob14//2010 midterm 6
using std::cout;
template<typename Op,typename T>
class Proxy{
	Op op;
	T val;
public:
	Proxy(Op _op, T _val) :op(_op), val(_val){}
	T operator()(const T& rhs){ return op(val, rhs); }

};
template<typename Op,typename T>
Proxy<Op,T> curry(Op op, T val){
	return Proxy<Op,T>{op, val};
}
int main(void){
	cout<<curry(std::multiplies<int>(), 4)(2);

	getchar();

}
#endif

#ifdef prob15//2010 midterm 7
using std::cout;
template<typename Op1,typename Op2,typename T>
class TwoProxy;

template<typename Op,typename T>

class Proxy{
	
public:
	Op op;
	T val;
	Proxy(Op _op, T _val) :op(_op), val(_val){}
	T operator()(const T& rhs){ return op(val, rhs); }
	template<typename Op2>
	TwoProxy<Op,Op2,T> operator+(Proxy<Op2,T>& rhs){
		return TwoProxy<Op, Op2, T>{*this, rhs};
	}


};

template<typename Op1,typename Op2,typename T>
class TwoProxy{
	Op1 op1;
	Op2 op2;
	T val1;
	T val2;
public:
	TwoProxy(Proxy<Op1, T>lhs, Proxy<Op2, T>rhs) :op1(lhs.op), val1(lhs.val), op2(rhs.op), val2(rhs.val){}
	T operator()(const T& arg){ return op2(val2, op1(val1, arg)); }


};
template<typename Op,typename T>
Proxy<Op,T> curry(Op op, T val){
	return Proxy<Op,T>{op, val};
}


int main(void){
	cout<<(curry(std::multiplies<int>(), 5)+curry(std::plus<int>(),3))(5);

	getchar();

}
#endif
