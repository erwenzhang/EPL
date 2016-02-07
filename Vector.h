// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _vector_h
#define _vector_h

namespace epl{
    
    static uint64_t unit_capacity = 8;
    template <typename T>
    class vector {
        uint64_t len;
        uint64_t capacity;
        T* data;
        T* dstart;
        T* dend;
        uint64_t start;
        uint64_t end;
        
    public:
        
        vector(void){
            len = 0;
            capacity = unit_capacity;
            data = reinterpret_cast<T*> (operator new(sizeof(T)*capacity ));
            dstart = data;
            dend = data;
            start = capacity/3;
            end = start;
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
                end = capacity;
               // end = 0;
            }
            
        }
        /*********************copy construcot and assignment**************************/
        vector(vector const& that){
            copy(that);
        }
        
        vector& operator=(vector const& rhs){
            if(this!= &rhs){
                destroy();
                copy(rhs);
            }
            return *this;
        }
        
        /*********************move construcot and assignment**************************/

        vector(vector&& that){
            this->my_move(std::move(that)); // why use this-> ?
        }
        
        vector& operator=(vector&& rhs){
            if(this!= &rhs){    // still need this condition or not?
                destroy();
                this->my_move(std::move(rhs));
            }
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
            uint64_t index = k + start;
            if(index>=capacity){
                index = index - capacity;
            }
            return data[index];
        }; //lvalue， can be modified
        
        const T & operator[](uint64_t k) const{
            if(k>=len){
                throw std::out_of_range("subscript ouf of range");
            }
            uint64_t index = k + start;
            if(index>=capacity){
                index = index - capacity;
            }
            return data[index];
        }; //rvalue, can not be modified
        
        void push_back(T const& val){
            if(len==capacity){
                uint64_t tmp_start = start;
                uint64_t tmp_end = end;
                uint64_t tmp_capacity = capacity;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;													// with invokes T::T(void) default contructor or not?
                end = start;
                
                for(uint64_t k = tmp_start;k<tmp_capacity;k+=1){
                    new (tmp_data+end) T{std::move(data[k])};
                    end++;
                }
                if(tmp_end!=tmp_capacity){
                    for(uint64_t k = 0;k<tmp_end;k+=1){
                        new (tmp_data+end) T{std::move(data[k])};
                        end++;
                    }
                }
                operator delete(data);
                data = tmp_data;
            }
            if(end==capacity) end = 0;
            new (data+end) T{val};
            end++;
            len++;
        }
        
        void push_back(T&& val){
            if(len==capacity){
                uint64_t tmp_start = start;
                uint64_t tmp_end = end;
                uint64_t tmp_capacity = capacity;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;													// with invokes T::T(void) default contructor or not?
                end = start;
                
                for(uint64_t k = tmp_start;k<tmp_capacity;k+=1){
                    new (tmp_data+end) T{std::move(data[k])};
                    end++;
                }
                if(tmp_end!=tmp_capacity){
                    for(uint64_t k = 0;k<tmp_end;k+=1){
                        new (tmp_data+end) T{std::move(data[k])};
                        end++;
                    }
                }
                operator delete(data);
                data = tmp_data;
                
            }
            if(end==capacity) end = 0;
            new (data+end) T{val};
            end++;
            len++;
        }
        
        
        void pop_back(void){
            if(len==0){
                throw std::out_of_range("there is no element for being poped");
            }
            if(end==0) end = capacity;
            end--;
            data[end].~T();
            len--;
            
        }
        
        void push_front(T const& val){
            if(len==capacity){
                uint64_t tmp_start = start;
                uint64_t tmp_end = end;
                uint64_t tmp_capacity = capacity;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it?
                start = capacity/3;
                end = start;
                
                for(uint64_t k = tmp_start;k<tmp_capacity;k+=1){
                    new (tmp_data+end) T{std::move(data[k])};
                    end++;
                }
                if(tmp_end!=tmp_capacity){
                    for(uint64_t k = 0;k<tmp_end;k+=1){
                        new (tmp_data+end) T{std::move(data[k])};
                        end++;
                    }
                }
                operator delete(data);
                data = tmp_data;
                
            }
            if(start == 0) start = capacity;
            start--;
            new (data+start) T(val);
            len++;
        }
        
        void push_front(T&& val){
            if(len==capacity){
                uint64_t tmp_start = start;
                uint64_t tmp_end = end;
                uint64_t tmp_capacity = capacity;
                capacity *= 2;
                T* tmp_data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));   // if it needs doubling, how to initialize it? 
                start = capacity/3;													// with invokes T::T(void) default contructor or not?
                end = start;
                
                for(uint64_t k = tmp_start;k<tmp_capacity;k+=1){
                    new (tmp_data+end) T{std::move(data[k])};
                    end++;
                }
                if(tmp_end!=tmp_capacity){
                    for(uint64_t k = 0;k<tmp_end;k+=1){
                        new (tmp_data+end) T{std::move(data[k])};
                        end++;
                    }
                }
                operator delete(data);
                data = tmp_data;
                
            }
            if(start == 0) start = capacity;
            start--;
            new (data+start) T{val};
            len++;
        }
        
        void pop_front(void){
            if(len==0){
                throw std::out_of_range("there is no element for being poped");
            }
            
            (data+start)->~T();
            start++;
            if(start==capacity) start = 0;
            len--;
            
        }
        
    private:
        void copy(vector const& that){
            this->len = that.len;
            this->capacity = that.capacity;
            data = reinterpret_cast<T*> (operator new(capacity * sizeof(T)));
            this->start = that.start;
            this->end = that.end;
            if(start<=end)
                for(uint64_t k = start;k<end;k+=1){
                    new (data+k) T{that.data[k]};
                }
            else{
                for(uint64_t k = 0;k<end;k+=1){
                     new (data+k) T{that.data[k]};
                }
                for(uint64_t k = start;k<capacity;k+=1){
                    new (data+k) T{that.data[k]};
                }
            }
            
        }
        
        void my_move(vector&& tmp) {
            std::cout << "wenwen's move!\n";
            this->data = tmp.data;
            this->len = tmp.len;
            this->capacity = tmp.capacity;
            this->start = tmp.start;
            this->end = tmp.end;
            tmp.data = nullptr;
        }
        
        void destroy(void){
            if(start<end){
                
                for(uint64_t k = start;k<end;k+=1){
                    (data+k)->~T();
                }
                
            }else if(start>=end&&len>0){
                
                for(uint64_t k = start;k<capacity;k+=1){
                    (data+k)->~T();
                }
                for(uint64_t k = 0;k<end;k+=1){
                    (data+k)->~T();
                }
                
                
            }
            
            
            operator delete (data);
            
        }
        
        
    };
    
} //namespace epl

#endif /* _vector_h */
