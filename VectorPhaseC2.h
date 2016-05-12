#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;

namespace epl{
    
    class invalid_iterator {
    public:
        enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
        SeverityLevel level;
        
        invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
        virtual const char* what() const {
            switch(level){
                case WARNING:   return "Warning"; // not used in Spring 2015
                case MILD:      return "Mild";
                case MODERATE:  return "Moderate";
                case SEVERE:    return "Severe";
                default:        return "ERROR"; // should not be used
            }
        }
    };
    
    static uint64_t unit_capacity = 2;
    template <typename T>
    class vector {
    private:
        uint64_t len;
        uint64_t capacity;
        T* data;
        T* dstart;
        T* dend;
        uint64_t start;
        uint64_t myend;
        
        uint64_t reallocate_times;
        uint64_t vector_version;
        
        
        
        
    public:
        vector(void){
            len = 0;
            capacity = unit_capacity;
            data = reinterpret_cast<T*> (operator new(sizeof(T)*capacity ));
            start = capacity/3;
            dstart = data+start;
            myend = start;
            dend = data+myend;
            
            reallocate_times = 0;
            vector_version = 0;
        }
        
        explicit vector(uint64_t n){
            if(n==0) vector(); // bad , right way?? or Vector()?
            else{
                len = n;
                capacity = n;
                data = reinterpret_cast<T*> (operator new(sizeof(T)*capacity));
                for(uint64_t k = 0;k<len;k+=1){
                    new(data+k) T();
                }
                //data = new T[n]; // not sure new T(n) or new T[n]
                start = 0;
                myend = capacity;
                dstart = data+start;
                dend = data+myend;
                
                reallocate_times = 0;
                vector_version = 0;
            }
            
        }
        /*********************copy construcot and assignment**************************/
        vector(vector<T> const& that){
            copy(that);
            reallocate_times = 0;
            vector_version = 0;
        }
        
        vector<T>& operator=(vector<T> const& rhs){
            if(this!= &rhs){
                destroy();
                copy(rhs);
            }
            ++reallocate_times;
            ++vector_version;
            return *this;
        }
        
        /*********************move construcot and assignment**************************/
        
        vector(vector<T>&& that){
            my_move(std::move(that)); // why use this-> ?
            
            reallocate_times = 0;
            vector_version = 0;
        }
        
        vector<T>& operator=(vector<T>&& rhs){
            if(this!= &rhs){    // still need this condition or not?
                destroy();
                my_move(std::move(rhs));
            }
            ++reallocate_times;
            ++vector_version;
            return *this;
        }
        
        
        
        ~vector(void){
            destroy();
        }
        
        uint64_t size(void) const{
            return this->len;
        }
        
        T& operator[](uint64_t k){
            if(k>=len){
                throw std::out_of_range("subscript ouf of range");
            }
//            uint64_t index = (k + start)%capacity;
//            // if(index>=capacity){
//            //     index = index - capacity;
//            // }
            return dstart[k];
        }; //lvalue， can be modified
        
         T& operator[](uint64_t k) const{
            if(k>=len){
                throw std::out_of_range("subscript ouf of range");
            }
//            uint64_t index = (k + start)%capacity;
//            // if(index>=capacity){
//            //     index = index - capacity;
//            // }
            return dstart[k];
        }; //rvalue, can not be modified
        
        void push_back(T const& val){
            if(dend == data+capacity){
                reallocate_times++;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;                                                 // with invokes T::T(void) default contructor or not?
                myend = start;
                new (tmp_data+start+len) T{val};
                for(uint64_t k = 0;k<len;k+=1){
                    new (tmp_data+myend) T{std::move(dstart[k])};
                    myend++;
                }
               
                myend++;
                len++;
                destroy();
                data = tmp_data;
                dstart = data+start;
                dend = data + myend;
            }else{
                
                new (data+myend) T{val};
                myend++;
                len++;
                dend = data + myend;
            }
            vector_version++;
            
        }
        
        void push_back(T&& val){
            if(dend == data+capacity){
                reallocate_times++;
//                uint64_t tmp_start = start;
//                uint64_t tmp_end = myend;
//                uint64_t tmp_capacity = capacity;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;                                                 // with invokes T::T(void) default contructor or not?
                myend = start;
                new (tmp_data+len+start) T{std::move(val)};
                for(uint64_t k = 0;k<len;k+=1){
                    new (tmp_data+myend) T{std::move(dstart[k])};
                    myend++;
                }
             
                myend++;
                len++;
                destroy();
                data = tmp_data;
                dstart = data+start;
                dend = data + myend;
                
            }else{
                new (data+myend) T{std::move(val)};
                myend++;
                len++;
                dend = data + myend;
                
            }
            
            vector_version++;
            
        }
        
        
      
        
        void push_front(T const& val){
            if(start == 0){
                reallocate_times++;
//                uint64_t tmp_start = start;
//                uint64_t tmp_end = myend;
//                uint64_t tmp_capacity = capacity;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;
                myend = start;
                
                new (tmp_data+start-1) T{val};
                
                for(uint64_t i= 0;i<len;i++){
                    new (tmp_data+myend) T{std::move(data[i])};
                    myend++;
                }
              
                start--;
                len++;
                destroy();
                data = tmp_data;
                dstart = data+start;
                dend = data + myend;
                
            }else{
              //  if(start == 0) start = capacity;
                start--;
                new (data+start) T{val};
                len++;
                dstart = data+start;
               
            }
            vector_version++;
            
        }
        
        void push_front(T&& val){
            if(dstart==data){
                reallocate_times++;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;                                                 // with invokes T::T(void) default contructor or not?
                myend = start;
                
                new (tmp_data+start-1) T{std::move(val)};
                
                for(uint64_t i= 0;i<len;i++){
                    new (tmp_data+myend) T{std::move(data[i])};
                    myend++;
                }
              
                start--;
                len++;
                destroy();
                data = tmp_data;
                dstart = data + start;
                dend = data + myend;
                
            }else{
               // if(start == 0) start = capacity;
                start--;
                new (data+start) T{std::move(val)};
                len++;
                dstart--;
               
            }
            vector_version++;
        }
        
        void pop_back(void){
            if(len==0){
                throw std::out_of_range("there is no element for being poped");
            }
            myend--;
            data[myend].~T();
            len--;
            dend--;
            
            vector_version++;
            
        }
        
        void pop_front(void){
            if(len==0){
                throw std::out_of_range("there is no element for being poped");
            }
            
            (data+start)->~T();
            start++;
            len--;
            dstart++;
          
            vector_version++;
            
        }
        
      

        class const_iterator;
        class iterator{
        private:
            T* ptr;
            uint64_t index;  // dstart+index,
            uint64_t it_version;
            uint64_t it_reallocate;
            const vector<T>* parent;
            bool flag; // already out_of bound or not
            
            
            
        public:
            friend const_iterator;
            friend vector;
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = int64_t;
            using pointer = T*;
            using reference = T&;
            
            iterator(void){
                ptr = NULL;
                parent = NULL;
                index = 0;
                it_version = 0;
                it_reallocate = 0;
                flag = true;
            }
            
            iterator(vector<T>* parent,T* ptr){
                this->ptr = ptr;
                this->parent = parent;
                it_version = parent->vector_version;
                it_reallocate = parent->reallocate_times;
                
                index = ptr - parent->dstart;
                
                update_flag();
                    
            
            }
            
            iterator(const iterator& rhs){
               rhs.check_invalid();
                ptr = rhs.ptr;
                index = rhs.index;
                it_version = rhs.it_version;
                it_reallocate = rhs.it_reallocate;
                flag = rhs.flag;
                parent = rhs.parent;
            }
            
            iterator& operator=(const iterator& rhs){
                if(this!=&rhs){
                    rhs.check_invalid();
                    ptr = rhs.ptr;
                    index = rhs.index;
                    it_version = rhs.it_version;
                    it_reallocate = rhs.it_reallocate;
                    parent = rhs.parent;
                    flag = rhs.flag;
                }
                return *this;
                
            }
            
            reference operator*(void) {
                check_invalid();
                return *ptr;
                }
            pointer operator->(void){ check_invalid(); return ptr;}
            reference operator[](difference_type n){ check_invalid(); return *(ptr+n);}
            
            
            iterator& operator++(void) { // pre ++
                check_invalid();  // to check whether the iterator is out_of_bound before using it
                index++;
                ptr++;
                update_flag();
                return *this;
                
            }
            iterator operator++(int) {  // post ++
                check_invalid();
                iterator t{*this}; // make a copy
                operator++(); // increment myself
                return t;
            }
            iterator operator+(difference_type k) {
                check_invalid();
                iterator t{*this};
                t.index = index+k;
                t.ptr = ptr + k;
                update_flag();
                return t;
                
            }
            iterator& operator+=(difference_type k) {
                check_invalid();
                
                index = index+k;
                ptr = ptr + k;
                update_flag();
                return *this;
                
            }
            
            iterator& operator--(void) { // pre--
                check_invalid();
                index--;
                ptr--;
                update_flag();
                return *this;
                
            }
            iterator operator--(int) {  // post--
                check_invalid();
                iterator t{*this}; // make a copy
                operator--(); // increment myself
                return t;
            }
            iterator operator-(difference_type k) {
                check_invalid();
                iterator t{*this};
                t.index = index - k;
                t.ptr = ptr - k;
                update_flag();
                return t;
                
            }
            iterator operator-=(difference_type k) {
                check_invalid();
                
                index = index-k;
                ptr = ptr - k;
                update_flag();
                return *this;
              
                
            }

            
            bool operator==(iterator const& rhs) const {
                check_invalid();
                 rhs.check_invalid();
                return this->ptr == rhs.ptr;
            }
            
            bool operator!=(iterator const& rhs) const {
                check_invalid();
                 rhs.check_invalid();
                return !(*this==rhs);
            }
            
            bool operator<(iterator const& rhs)const{
                 check_invalid();
                 rhs.check_invalid();
                return index<rhs.index;
            }
            bool operator>(iterator const& rhs)const{
                check_invalid();
                 rhs.check_invalid();
                return index>rhs.index;
            }
            bool operator<=(iterator const& rhs)const{
                check_invalid();
                 rhs.check_invalid();
                return !(*this>rhs);
            }
          
            bool operator>=(iterator const& rhs)const{
                check_invalid();
                rhs.check_invalid();
                return index>=rhs.index;
            }
            
            difference_type operator-(iterator& it){
                check_invalid();
                it.check_invalid();
                return this->ptr - it.ptr;
            }
            
            void check_invalid() const{
                if(it_version != parent->vector_version){
                    
                    if( ((index < 0)||(index > parent->size() )))
                        throw epl::invalid_iterator{    epl::invalid_iterator::SEVERE };
                    else if(it_reallocate != parent->reallocate_times&&index>=0&&index<=parent->size())
                        throw epl::invalid_iterator{    epl::invalid_iterator::MODERATE };
                    else
                        throw epl::invalid_iterator{    epl::invalid_iterator::MILD  };
                }
            
            }
            void update_flag(){
                if(index>=0&&index<=parent->size())
                    flag = true;
                else
                    flag = false;
            
            }
            
        };
        class const_iterator{
        private:
            const T* ptr;
            uint64_t index;  // dstart+index,
            uint64_t it_version;
            uint64_t it_reallocate;
            const vector<T>* parent;
            bool flag; // already out_of bound or not
            
            
        public:
            friend vector;
            using iterator_category = std::random_access_iterator_tag;
            using value_type = const T;
            using difference_type = int64_t;
            using pointer = const T*;
            using reference = const T&;
            
            const_iterator(void){
                ptr = NULL;
                parent = NULL;
                index = 0;
                it_version = 0;
                it_reallocate = 0;
                flag = true;
            }
            
            const_iterator(const vector<T>* parent,const T* ptr){
                this->ptr = ptr;
                this->parent = parent;
                it_version = parent->vector_version;
                it_reallocate = parent->reallocate_times;
                
                index = ptr - parent->dstart;
                
                update_flag();
                
                
            }
            
            const_iterator(const iterator& rhs){
                rhs.check_invalid();
                ptr = rhs.ptr;
                index = rhs.index;
                it_version = rhs.it_version;
                it_reallocate = rhs.it_reallocate;
                flag = rhs.flag;
                parent = rhs.parent;
            }
            
            const_iterator(const const_iterator& rhs){
                
                rhs.check_invalid();
                ptr = rhs.ptr;
                index = rhs.index;
                it_version = rhs.it_version;
                it_reallocate = rhs.it_reallocate;
                parent = rhs.parent;
                flag = rhs.flag;
                
                
            }

            const_iterator& operator=(const const_iterator& rhs){
                if(this!=&rhs){
                    rhs.check_invalid();
                    ptr = rhs.ptr;
                    index = rhs.index;
                    it_version = rhs.it_version;
                    it_reallocate = rhs.it_reallocate;
                    parent = rhs.parent;
                    flag = rhs.flag;
                }
                return *this;
                
            }
            
            reference operator*(void) const{
                check_invalid();
                return *ptr;
            }
            pointer operator->(void) const { check_invalid(); return ptr;}
            reference operator[](difference_type n) const { check_invalid(); return *(ptr+n);}
            
            
            const_iterator& operator++(void) { // pre ++
                check_invalid();  // to check whether the iterator is out_of_bound before using it
                index++;
                ptr++;
                update_flag();
                return *this;
                
            }
            const_iterator operator++(int) {  // post ++
                check_invalid();
                const_iterator t{*this}; // make a copy
                operator++(); // increment myself
                return t;
            }
            const_iterator operator+(difference_type k) {
                check_invalid();
                const_iterator t{*this};
                t.index = index+k;
                t.ptr = ptr + k;
                update_flag();
                return t;
                
            }
            const_iterator& operator+=(difference_type k) {
                check_invalid();
                
                index = index+k;
                ptr = ptr + k;
                update_flag();
                return *this;
                
            }
            
            const_iterator& operator--(void) { // pre--
                check_invalid();
                index--;
                ptr--;
                update_flag();
                return *this;
                
            }
            const_iterator operator--(int) {  // post--
                check_invalid();
                const_iterator t{*this}; // make a copy
                operator--(); // increment myself
                return t;
            }
            const_iterator operator-(difference_type k) {
                check_invalid();
                const_iterator t{*this};
                t.index = index - k;
                t.ptr = ptr - k;
                update_flag();
                return t;
                
            }
            const_iterator& operator-=(difference_type k) {
                check_invalid();
                
                index = index-k;
                ptr = ptr - k;
                update_flag();
                return *this;
                
                
            }
            
            
            bool operator==(const const_iterator & rhs) const {
                check_invalid();
                rhs.check_invalid();
                return this->ptr == rhs.ptr;
            }
            
            bool operator!=(const const_iterator & rhs) const {
                check_invalid();
                rhs.check_invalid();
                return !(*this==rhs);
            }
            
            bool operator<(const_iterator const& rhs)const{
                check_invalid();
                rhs.check_invalid();
                return index<rhs.index;
            }
            bool operator>(const_iterator const& rhs)const{
                check_invalid();
                rhs.check_invalid();
                return index>rhs.index;
            }
            bool operator<=(const_iterator const& rhs)const{
                check_invalid();
                rhs.check_invalid();
                return !(*this>rhs);
            }
            
            bool operator>=(const_iterator const& rhs)const{
                check_invalid();
                rhs.check_invalid();
                return index>=rhs.index;
            }
            
            difference_type operator-(const_iterator const& it){
                check_invalid();
                it.check_invalid();
                return this->ptr - it.ptr;
            }
            
            void check_invalid() const{
                if(it_version != parent->vector_version){
                    
                    if( ((index < 0)||(index > parent->size() )) && flag)
                        throw epl::invalid_iterator{    epl::invalid_iterator::SEVERE };
                    else if(it_reallocate != parent->reallocate_times&&index>=0&&index<=parent->size())
                        throw epl::invalid_iterator{    epl::invalid_iterator::MODERATE };
                    else
                        throw epl::invalid_iterator{    epl::invalid_iterator::MILD  };
                }
            
            }
            void update_flag(){
                if(index>=0&&index<=parent->size()){
                    flag = true;
                }else
                    flag = false;
                
            }

        
        };

          /*********************member template constructor [b,e)**************************/
        template<typename iter>
        vector(iter b,iter e){
            using _it_category = typename std::iterator_traits<iter>::iterator_category;
            
            initialize_dispatch(b,e,_it_category{});
//   both ways work fine
//      typename std::iterator_traits<iter>::iterator_category x{};
//           initialize_dispatch(b, e, x);
        
        }
        template<typename iter>
        void initialize_dispatch(iter b,iter e,std::input_iterator_tag){
            std::cout<<"wenwen"<<std::endl;
            capacity = unit_capacity;
            data = reinterpret_cast<T*>(operator new(sizeof(T)* capacity));
            dstart = data;
            start = 0;
            myend = 0;
            len = 0;         
            dend = data;
            for(;b!=e;b++){
                push_back(*b);
                
            }
            reallocate_times = 0;
            vector_version = 0;
            
            
        
        }
        template<typename iter>
        void initialize_dispatch(iter b,iter e,std::random_access_iterator_tag){
            len  = e - b;
            capacity = len;
            data = reinterpret_cast<T*>(operator new(sizeof(T)* capacity));
            dstart = data;
            dend = data+len;
            start = 0;
            myend = len;
            vector_version = 0;
            reallocate_times = 0;
            for(int i = 0;i<len;i++){
                new(dstart+i)T{*(b+i)};
            }
            
        }
        
        /*********************constructor from std::initializer_list<T>**************************/
        
        vector(std::initializer_list<T> list):vector(list.begin(),list.end()){}
        
        
        /*********************begin(),end() function**************************/
        iterator begin(void){
            return iterator{this,dstart};
        }
        const_iterator begin(void) const{
            return const_iterator{this, dstart};
            
        }
        iterator end(void){
            return iterator{this,dend};
        }
        
        const_iterator end(void) const{
            return const_iterator{this,dend};
        }
        
        /*********************emplace_back variadic member template function**************************/
        template<typename... Args>
        void emplace_back(Args&&... args ){
            if(dend==data+capacity){
                reallocate_times++;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity*sizeof(T)));
                start = capacity/3;
                myend = start;
                new(tmp_data+len)T{std::forward<Args>(args)...};

                for(int i = 0;i<len;i++){
                    new(tmp_data+myend) T{std::move(*(data+i))};
                    myend++;
                }
                len++;
                myend = len;
                destroy();
                data = tmp_data;
                dstart = tmp_data + start;
                dend = tmp_data + myend; 


            }else{
                new (data+len) T{ std::forward<Args>(args)... };
                len++;
                dend++;
                myend++;

            }
            vector_version++;
            
        }
        
    private:
        void copy(vector<T> const& that){
            this->len = that.len;
            this->capacity = that.capacity;
            data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));
            this->start = that.start;
            this->myend = that.myend;
            this->dstart = data+start;
            this->dend = data+myend;
            
          
//            if(start<=myend)
                for(uint64_t k = start;k<myend;k+=1){
                    new (data+k) T{that.data[k]};
                }
//            else{
//                for(uint64_t k = 0;k<myend;k+=1){
//                    new (data+k) T{that.data[k]};
//                }
//                for(uint64_t k = start;k<capacity;k+=1){
//                    new (data+k) T{that.data[k]};
//                }
//            }
            
        }
        
        void my_move(vector<T>&& tmp) {
          
            this->data = tmp.data;
            this->len = tmp.len;
            this->capacity = tmp.capacity;
            this->start = tmp.start;
            this->myend = tmp.myend;
            this->dstart = tmp.dstart;
            this->dend = tmp.dend;
           
         
            
            tmp.data = nullptr;
            tmp.myend = 0;
            tmp.start = 0;
            tmp.len = 0;
            tmp.capacity = 0;
            tmp.dstart = nullptr;
            tmp.dend = nullptr;
            tmp.vector_version++;
            tmp.reallocate_times++;
        }
        
        void destroy(void){
//            if(start<myend){
            
            T* it = dstart;
            while(it!=dend){
                it->~T();
                it++;
            }
            
//            }else if(start>=myend&&len>0){
//                
//                for(uint64_t k = start;k<capacity;k+=1){
//                    (data+k)->~T();
//                }
//                for(uint64_t k = 0;k<myend;k+=1){
//                    (data+k)->~T();
//                }
//                
//                
//            }
            
            
            operator delete (data);
            
        }
        
        
    };
    
} //namespace epl

#endif
