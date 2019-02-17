#include <iostream>
#include "poc.hpp"
#include <type_traits>


#include <tuple>
#include <functional>
#include <iostream>

template<int N>
struct Case {
     enum { value = N };
     static void proc()
     {
        std::cout << "[ XXX Case XXX ] " << value << std::endl;
        //return 4;
     }
};

typedef std::function<void(void)> F_t;
typedef Case<1>  Case_1;
typedef Case<2>  Case_2;
typedef Case<3>  Case_3;
struct EndOF {};
#define L1(x) std::tuple<x , EndOF >
#define L2(x,y) std::tuple<x, L1(y) >
#define L3(x,y,z) std::tuple<x, L2(y,z) >


  template< bool B, class T = void >
  using enable_if_t = typename std::enable_if<B,T>::type;

struct NullType {};

template<bool v , class T>
struct enable_if {
      typedef NullType type;
};


template <class T>
struct enable_if<true,T> {
      typedef T type;
};



template<typename T>
class MyClass
{
public:
    void f(T const& x);

    template<typename T_ = T>
    void f(T&& x,
           typename std::enable_if<!std::is_reference<T_>::value,
           std::nullptr_t>::type = nullptr);
};



 template<class T>
 struct Class {

    typedef typename std::add_lvalue_reference<T>::type E;

    template<class TT>
    void run(TT p, typename std::enable_if< std::is_same<TT,int>::value,T>::type* = nullptr )
    {
         std::cout << "int " << std::endl;
    }

    template<class TT>
    void run(TT p, typename std::enable_if< std::is_same<TT,float>::value,T>::type* = nullptr )
    {
         std::cout << "float " << std::endl;
    }

    template<class TT>
    void run(TT p, typename std::enable_if< std::is_same<TT,double>::value,T>::type* = nullptr )
    {
             std::cout << "double " << std::endl;
    }


    // void run(T p,typename std::enable_if<false,T>::type* =nullptr)
    // {
    //      std::cout << "float " << std::endl;
    // }


    //
    // void run(E p )
    // {
    //      std::cout << "double " << std::endl;
    // }
    //
    // void run(E p)
    // {
    //      std::cout << "float " <<  std::endl;
    // }




          // void run(T p)
          // {
          //   std::cout << "Copy " << std::endl;
          // }

/*
         template<class T_ = T >
         void run(T p, typename std::enable_if<true,T_>::type = 0   ) {

               std::cout << "Reference" << std::endl;
         }
*/
         //
        //  template<class T_ , typename enable_if<false, typename std::add_lvalue_reference<T>::type>::type >
        //  void run(T_ p) {
        //         std::cout << "By reference" << std::endl;
        //  }


 };






int main(int argc, char **argv) {

  float c = 3.0;

  Class<float> cc;
  cc.run(c);

    return 0;
}
