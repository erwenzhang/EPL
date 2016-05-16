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
