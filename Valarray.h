
// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h

#include <iostream>
#include <vector>
#include <algorithm>
#include "Vector.h"
#include <complex>

using std::complex;
//using std::vector; // during development and testing
using epl::vector; // after submission
//namespace epl {
template<typename T> struct SRank;
template<typename T>
class ScalarWrapper;
template<typename T1,typename T2,typename Op>
class BinaryProxy;
template<typename T,typename Op>
class UnaryProxy;


template<bool B,class T = void> struct operation_enable_if{};
template<class T>  struct operation_enable_if<true,T>{using type = T;};
template<class T>  struct operation_enable_if<false,T>{};
// template<typename T>
// using enable_if = typename operation_enable_if<SRank<T>::value!=0,T>::type;




/****************************for function overload *********************************************/
template<typename T> struct Is_Scalar;
template<> struct Is_Scalar<int>{using type = int;};
template<> struct Is_Scalar<double>{using type = double;};
template<> struct Is_Scalar<float>{using type = float;};
template<> struct Is_Scalar<complex<double>>{using type = complex<double>;};
template<> struct Is_Scalar<complex<float>>{using type = complex<float>;};

/************************************choose type*****************************************************/


template<typename T> struct SRank{static constexpr int value = 0;static constexpr bool flag = false;};
template<>struct SRank<int>{ static constexpr int value = 1;static constexpr bool flag = false;};
template<>struct SRank<float>{static constexpr int value = 2;static constexpr bool flag = false;};
template<>struct SRank<double>{static constexpr int value = 3;static constexpr bool flag = false;};
template<typename T>
struct SRank<complex<T>>{static constexpr int value = SRank<T>::value; static constexpr bool flag = true;};


template<int>struct SType;
template<>struct SType<1>{using type = int;};
template<>struct SType<2>{using type = float;};
template<>struct SType<3>{using type = double;};

template<typename T, bool is_complex> struct CType;
template<typename T> struct CType<T,false>{using type = T;};
template<typename T> struct CType<T,true>{using type =complex<T>;};

template <typename T1,typename T2>
struct choose_type{
    static constexpr int t1_rank = SRank<T1>::value;
    static constexpr int t2_rank = SRank<T2>::value;
    static constexpr int max_rank = (t1_rank > t2_rank)? t1_rank: t2_rank;
    static constexpr bool is_complex = SRank<T1>::flag||SRank<T2>::flag;
    using type = typename CType<typename SType<max_rank>::type,is_complex>::type;
};


template <typename T1,typename T2>
using ChooseType = typename choose_type<T1,T2>::type;

template<typename T>struct is_complex{static constexpr bool flag = false;};
template<> struct is_complex<complex<int>>{static constexpr bool flag = true;};
template<> struct is_complex<complex<float>>{static constexpr bool flag = true;};
template<> struct is_complex<complex<double>>{static constexpr bool flag = true;};

/**********************************choose ref or not***************************************************/
template<typename T>
struct chooseRef{
    using type = T;
};

template<typename T>
struct chooseRef<vector<T>>{
    using type = vector<T> const&;
};

template<typename T>
using ChooseRef = typename chooseRef<T>::type;

/******************************iterator********************************************/

template<typename Proxy>
class MyIterator{
    const Proxy parent;
    uint64_t index;
public:
    using iterator_category = std::random_access_iterator_tag;
    //using T = typename Proxy::value_type;
    using T = typename Proxy::result_type;
    using difference_type = uint64_t;
    using value_type = const T;
    using pointer = const T*;
    using reference = const T; // must be value!!!!!!!! cannot be reference,
    
    MyIterator(const Proxy& _parent,const uint64_t _index):parent(_parent),index(_index){    }
    
    
    
    reference operator*(void) const{
        return parent[index];
    }
    
    MyIterator& operator++(void) { // pre ++
        index++;
        return *this;
        
    }
    MyIterator operator++(int) {  // post ++
        MyIterator t{*this}; // make a copy
        operator++(); // increment myself
        return t;
    }
    MyIterator operator+(difference_type k) {
        MyIterator t{*this};
        t.index = index+k;
        return t;
        
    }
    
    
    MyIterator& operator--(void) { // pre--
        
        index--;
        return *this;
        
    }
    MyIterator operator--(int) {  // post--
        MyIterator t{*this}; // make a copy
        operator--(); // increment myself
        return t;
    }
    MyIterator operator-(difference_type k) {
        MyIterator t{*this};
        t.index = index - k;
        return t;
        
    }
    
    bool operator==(const MyIterator & rhs) const {
        
        return this->index == rhs.index;
    }
    
    bool operator!=(const MyIterator & rhs) const {
        
        return this->index != rhs.index;
    }
    
    bool operator<(MyIterator const& rhs)const{
        return index<rhs.index;
    }
    bool operator>(MyIterator const& rhs)const{
        rhs.check_invalid();
        return index>rhs.index;
    }
    bool operator<=(MyIterator const& rhs)const{
        
        rhs.check_invalid();
        return !(*this>rhs);
    }
    
    bool operator>=(MyIterator const& rhs)const{
        
        return index>=rhs.index;
    }
    
    
    
};

/**********************BinaryProxy**********************************************/
template<typename V1Type, typename V2Type, typename Op>
class BinaryProxy{
public:
    using LeftType = ChooseRef<V1Type>;
    using RightType = ChooseRef<V2Type>;
    LeftType v1;
    RightType v2;
    Op op;
    using value_type = typename choose_type<typename V1Type::value_type,typename V2Type::value_type>::type;
    using result_type = value_type;
    
public:
    
    
    BinaryProxy(V1Type const& lhs, V2Type const& rhs,Op operation):v1(lhs),v2(rhs),op(operation){}
    
    BinaryProxy(const BinaryProxy& that):v1(that.v1),v2(that.v2),op(that.op){}
    
    uint64_t size() const{
        return std::min(v1.size(),v2.size());
    }
    
    typename Op::result_type operator[](uint64_t k)const{
        return op(v1[k],v2[k]);
    }
    
    using const_iterator = MyIterator<BinaryProxy>;
    
    const_iterator begin()const{
        return MyIterator<BinaryProxy> {*this,0};
    }
    
    const_iterator end()const{
        return MyIterator<BinaryProxy> {*this,this->size()};
    }
    
    
};
/**********************UnaryProxy**********************************************/
template<typename T,typename Op>
class UnaryProxy{
public:
    using Type = ChooseRef<T>;
    Type v;
    Op op;
    using value_type = typename T::value_type;
    using result_type = ChooseType<typename T::value_type,typename Op::result_type>;
    
public:
    
    
    UnaryProxy(T const& _v,Op operation):v(_v),op(operation){}
    UnaryProxy(const UnaryProxy& that):v(that.v),op(that.op){}
    uint64_t size()const{return v.size();}
    result_type operator[](uint64_t k)const{
        return op(v[k]);
    }
    
    using const_iterator = MyIterator<UnaryProxy>;
    
    const_iterator begin()const{
        return MyIterator<UnaryProxy> {*this,0};
    }
    
    const_iterator end()const{
        return MyIterator<UnaryProxy> {*this,this->size()};
    }
    
    
};
/*********************************scalar*****************************************************/
template<typename T>
class ScalarWrapper{
    T member;
    
public:
    using value_type = T;
    
    ScalarWrapper():member(0){}
    ScalarWrapper(const T& arg):member(arg){}
    ScalarWrapper(const ScalarWrapper& that):member(that.member){}
    T operator[](uint64_t k)const{return member;}
    uint64_t size()const{ return std::numeric_limits<uint64_t>::max();}
    
    using const_iterator = MyIterator<ScalarWrapper>;
    const_iterator begin()const{return const_iterator {*this, 0}; }
    const_iterator end()const{return const_iterator{*this, this->size()}; }
    
};
/***************function object*****************/
template<typename T>
struct Sqrt{
    
    using result_type = ChooseType<T,double>;
    result_type operator()(T val)const{
        return sqrt(val);
    }

};
/*****************************wrap************************************************/
template<typename T>
struct Wrap:public T{
    using T::T;
public:
    Wrap():T(){}; // valarray<double> x,y,z;
    Wrap(const T& t):T(t){} //valarray<T> x(5); copy constructor belongs to every class itself, cannot be inherited
    explicit Wrap(uint64_t size):T(size){}
    
    template<typename RHS>
    Wrap(const Wrap<RHS> & that){ //x = y+z; LHS is Wrap<MyVector<T>>, RHS
        for(auto val:that){
            this->push_back(static_cast<typename T::value_type>(val));
        }
    }
    
    /********************assignment to wrap<T> **********************/
    Wrap<T>& operator=(const Wrap<T>& that){
        if((void*)this!=(void*)&that){
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for(uint64_t k = 0;k<min_size;k++){
                (*this)[k] = static_cast<typename T::value_type>(that[k]);
                
            }
        }
        return *this;
    }
    
    /********************assignment to wrap<AnyType> **********************/
    template<typename RHS1>
    Wrap<T>& operator=(const Wrap<RHS1>& that){
        if((void*)this!=(void*)&that){
            uint64_t min_size = this->size() < that.size() ? this->size() : that.size();
            for(uint64_t k = 0;k<min_size;k++){
                (*this)[k] = static_cast<typename T::value_type>(that[k]);
                
            }
        }
        return *this;
    }
    
    /********************assignment with scalar **********************/
    template<typename RHS2>
    typename std::enable_if<SRank<RHS2>::value,Wrap<T>>::type & operator=(const RHS2& that){
        for(uint64_t k = 0;k<this->size();k++){
            (*this)[k] = static_cast<typename T::value_type>(that);
            
        }
        
        return *this;
    }
    /****************************sum***********************************/
    typename T::value_type sum(){
//        typename T::value_type result = 0;
//        for(uint64_t k = 0;k<this->size();k++){
//            result+=(*this)[k];
//        }
//        return result;
        return accumulate(std::plus<typename T::value_type>{});
    }
    /****************************accumulate*******************************/
    template<typename Op>
    typename Op::result_type accumulate(Op op){
        if(this->size()<=0) return 0;
        typename Op::result_type result = (*this)[0];
        for(uint64_t k = 1;k<this->size();k++){
              result = op((*this)[k],result);
        }
        return result;
    }
    /*********************apply**********************/
    template<typename Op>
    Wrap<UnaryProxy<T,Op>> apply(Op op){
        
        return Wrap<UnaryProxy<T,Op>>{*this,op};
    }
    /************************sqrt********************/
    //template<typename Op = >
    //auto sqrt() ->decltype(apply(Sqrt<typename T::value_type>{})){
    Wrap<UnaryProxy<T,Sqrt<typename T::value_type>>> sqrt(){
        return  apply(Sqrt<typename T::value_type>{});
    }
    
    
};
template<typename T>
using valarray = Wrap<vector<T>>;


/**********************apply_op**********************************************/


template <typename Op, typename T1, typename T2>
Wrap<BinaryProxy<T1,T2,Op>> apply_op(Wrap<T1> const& x, Wrap<T2> const& y, Op op = Op{}) {
    T1 const& lhs{x};
    T2 const& rhs{y};
    BinaryProxy<T1, T2, Op> result{lhs,rhs,op};
    return Wrap<BinaryProxy<T1, T2, Op>>{result};
}

template <typename Op, typename T1, typename T2>
typename operation_enable_if<SRank<T2>::value!=0,Wrap<BinaryProxy<T1, ScalarWrapper<T2>, Op>>>::type apply_op(Wrap<T1> const& x, T2 const& y, Op op = Op{}) {
    T1 const& lhs{x};
    ScalarWrapper<T2> const& rhs{y};
    BinaryProxy<T1, ScalarWrapper<T2>, Op> result{lhs,rhs,op};
    return Wrap<BinaryProxy<T1, ScalarWrapper<T2>, Op>>{result};
}

template <typename Op, typename T1, typename T2>
typename operation_enable_if<SRank<T1>::value!=0,Wrap<BinaryProxy<ScalarWrapper<T1>, T2, Op>>>::type apply_op(T1 const& x, Wrap<T2> const& y, Op op = Op{}) {
    ScalarWrapper<T1> const& lhs{x};
    T2 const& rhs{y};
    BinaryProxy<ScalarWrapper<T1>, T2, Op> result{lhs,rhs,op};
    return Wrap<BinaryProxy<ScalarWrapper<T1>, T2, Op>>{result};
}
/********************************* return type *********************************************************/
template<typename T1,typename  T2>
struct ReturnType{};
template<typename T1,typename T2>
struct ReturnType<Wrap<T1>,Wrap<T2>>{using type = ChooseType<typename T1::value_type,typename T2::value_type>;};
template<typename T1,typename T2>
struct ReturnType<T1,Wrap<T2>>{ using type = ChooseType<T1,typename T2::value_type>;};
template<typename T1,typename T2>
struct ReturnType<Wrap<T1>,T2>{ using type = ChooseType<typename T1::value_type,T2>;};
/*********************************operator overload****************************************************/

template<typename T1,typename T2>
auto operator+(const T1&lhs,const T2&rhs)->decltype(apply_op(lhs,rhs,std::plus<typename ReturnType<T1,T2>::type>{})){
    
    return apply_op(lhs,rhs,std::plus<typename ReturnType<T1,T2>::type>{});
}
template<typename T1,typename T2>
auto operator-(const T1&lhs,const T2& rhs)->decltype(apply_op(lhs,rhs,std::minus<typename ReturnType<T1,T2>::type>{})){
    
    return apply_op(lhs,rhs,std::minus<typename ReturnType<T1,T2>::type>{});
}
template<typename T1,typename T2>
auto operator*(const T1& lhs,const T2& rhs)->decltype(apply_op(lhs,rhs,std::multiplies<typename ReturnType<T1,T2>::type>{})){
    return apply_op(lhs,rhs,std::multiplies<typename ReturnType<T1,T2>::type>{});
}
template<typename T1,typename T2>
auto operator/(const T1& lhs,const T2& rhs)->decltype(apply_op(lhs,rhs,std::divides<typename ReturnType<T1,T2>::type>{})){
    
    return apply_op(lhs,rhs,std::divides<typename ReturnType<T1,T2>::type>{});
}

template<typename T>
Wrap<UnaryProxy<T,std::negate< typename T::value_type>>> operator-(const Wrap<T>& arg){
    return Wrap<UnaryProxy<T, std::negate<typename T::value_type>>>{arg, std::negate<typename T::value_type>{}};
}

template<typename T>
std::ostream& operator<<(std::ostream& out,const Wrap<T> t){
    out<<"{ ";
    for(uint64_t k = 0;k<t.size();++k){
        out<<t[k]<<", ";
    }
    out<<" }";
    return out;
}


#endif /* _Valarray_h */


