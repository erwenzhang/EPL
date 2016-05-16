#include<vector>
using std::ostream;
using std::cout;
using std::endl;


class Foo{
	//char let;
public:
	char let;
	Foo(char v){let=v;}

};

class Proxy{
	std::vector<Foo> v;
	ostream& out;
public:
	Proxy(ostream& _out,Foo arg):out(_out){
		v.push_back(arg);
		//this->out = out;
	}

	Proxy& operator<<(Foo arg){
		v.push_back(arg);	
		return *this;
	}
	template<typename T>
	ostream& operator<<(T arg){
		int size = v.size();
		for (int i = size - 1; i >= 0; i--){
			out << v[i].let;
		}
		return out;
	}
};

Proxy operator<<(ostream& out,Foo arg){
	Proxy p{ out,arg};
	//p.print(out);
	return p;
}

int main(void){
	char endl = '\n';
	cout << "hello";
	Foo a('A'),b('b'),c('c');
	cout<<a<<b<<c<<endl;
	getchar();
	return 0;
}
