#include <iostream>
#include "poc.hpp"



#include <tuple>
#include <functional>
#include <iostream>

template<int N>
struct Case {
     enum { value = N };
     static void proc()
     {
        std::cout << "Case " << value << std::endl;
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



template<class L>
struct Switch;

struct Default
{
     static void proc()
     {
         std::cout << "Default case" << std::endl;
     }
};
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


int main(int argc, char **argv) {


#define MY

    int c = 3;
#ifdef MY
       Switch<L3(Case_1,Case_2,Case_3)>::run(c);
#else
     switch(c)
     {
        case 1:
             Case_1::proc();
        break;

        case 2:
             Case_2::proc();
        break;

        case 3:
              Case_3::proc();
        break;

        default:
            Default::proc();
     }
#endif
    return 0;
}
