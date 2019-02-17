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
        std::cout << "Case " << value << std::endl;
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


//
// template<class L>
// struct Switch;

struct Default
{
     static void proc(int size)
     {
         std::cout << "Default case" << size <<  std::endl;
      //   return 3;
     }
};

/*
template<class H, class T>
struct Switch< std::tuple<H,T> >
{
     static void run(int check)
     {
         ( H::value == check ) ? H::proc() : Switch<T>::run(check);
     }
};

template<class H>
struct Switch< std::tuple<H,EndOF> >
{
     static void run(int check)
     {
         ( H::value == check ) ? H::proc() : Default::proc();
     }
};
*/

template<class W>
struct Wrap {
  typedef  void type ;
};


template<int K >
struct Int2Int {
  enum { value = K };
};

template<class H, class ... Tail>
struct Switch
{
   enum { size = sizeof ...(Tail) };
  //
  // template<class I, class = void >
  // static void run(I check)
  // {
  //     ( H::value == check ) ? H::proc() : Default::proc(check);
  // }
  //
  // template<class I, typename std::enable_if< (size > 0),int>::type >
  // static void run(I check)
  // {
  //      ( H::value == check ) ? H::proc() : Switch<Tail...>::run(check);
  // }



    template<class I, class=void>
    static void run(I check)
    {
        ( H::value == check ) ? H::proc() : Default::proc(check);
    }

    template< class I, typename std::enable_if< (size == 1 ),I>::type >
    static void run(I check)
    {
              ( H::value == check ) ? H::proc() : Switch<Tail...>::run(check);
    }

    template< class I, typename std::enable_if< (size > 1 ),I>::type >
    static void run(I check)
    {
              ( H::value == check ) ? H::proc() : Switch<Tail...>::run(check);
    }




    //
    // template< class I>
    // static typename Wrap<typename std::enable_if< (size > 1),I>::type>::type  run(I check)
    // {
    //           ( H::value == check ) ? H::proc() : Switch<Tail...>::run(check);
    // }


   //
   //
  //  template< class I>
  //  static typename std::enable_if< (size == 1),I>::type  run(I check)
  //   {
  //                  ( H::value == check ) ? H::proc() : Switch<Tail...>::run(check);
  //   }
   //





};
//
//
// template<int N, class K = void>
// struct kupa {
//   enum { value = 0 };
// };
//
// template<int N>
// struct kupa <N, typename std::enable_if< (N > 20) >::type >  {
//
// enum { value = 1 };
//
// };
//
//
// template<class R>
// R fun(int k)
// {
//     return 7;
// }
//
// typename std::enable_if<true,int>::type fun(int k)
// {
//    return 4;
// }
//
// typename std::enable_if<false,int>::type fun(int k)
// {
//    return 4;
// }



int main(int argc, char **argv) {

//  kupa<10> w;
  int c = 7;


 Switch<Case_1,Case_2,Case_3,Case_3>::run(c);

//
// #define MY
//
//     int c = 3;
// #ifdef MY
//        Switch<Case_1,Case_2,Case_3>::run(c);
// #else
//      switch(c)
//      {
//         case 1:
//              Case_1::proc();
//         break;
//
//         case 2:
//              Case_2::proc();
//         break;
//
//         case 3:
//               Case_3::proc();
//         break;
//
//         default:
//             Default::proc();
//      }
// #endif
    return 0;
}
