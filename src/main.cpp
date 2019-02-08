#include <iostream>
#include <string.h>
#include <algorithm>

namespace fmt {
     template<int V>
     struct address { enum { value = V };  };
}


template<int N>
std::ostream& operator<<(std::ostream& o, const fmt::address<N>& )
{
   return o << std::hex << " 0x" << fmt::address<N>::value;
}

template<class F>
void log_internal(std::ostream& o, F&& f)
{
    o << std::forward<F>(f);
}

template<class F, class ... T>
void log_internal(std::ostream& o, F&& f, T&&...t)
{
    o << std::forward<F>(f);
    log_internal(o, std::forward<T>(t)...);
}

template<class...T>
void log(T&&...args)
{
    std::cout << "[LOG]: ";
    log_internal(std::cout, std::forward<T>(args)...);
    std::cout << std::endl;
}


template<class R>
struct Ret {

         const char* err;
         R* value;

     Ret() : err(0) {}
     operator R() const { return *value; }
};



template<int H, int W, class T=unsigned char>
class Matrix {
protected:
       typedef T Array[H][W];
       Array tab;
public:
       Matrix() { memset(tab,0,sizeof(tab)); }
       T* begin() { return &tab[0][0]; }
       T* end() { return &tab[H-1][W]; }

       Array* raw()  { return &tab; }

       template<int x, int y>
       T static_get()
       {
           static_assert( (x < H && x >=0 ) , "Index array[X][..] out of range");
           static_assert( (y < W && y >= 0) , "Index array[...][Y] out of range");
           return tab[x][y];
       }

       Ret<T> get(size_t x, size_t y)
       {
           Ret<T> r;
           if(!(x >=0 && y>=0 && x<H && y<W))
           {
               r.err = "get(x,y) Index of out range";
               return r;
           }
           r.value = &tab[x][y];
           return r;
       }

        void for_each_pixel(std::function<void(T)> f)
        {
             for(size_t i=0;i<H*W;++i) f( reinterpret_cast<T*>(tab)[i])  ;
        }

};

int main(int argc, char **argv)
{


    log("------------------");
    Matrix<6,6> matrix;
    //matrix.for_each_pixel( [](unsigned char pixel){  if(pixel) std::cout << "wrong" << std::endl;   });

   std::for_each(matrix.begin(),matrix.end(),[](unsigned char pixel){  std::cout << "pixel" << (int)pixel << std::endl;  });
    //auto p =  matrix.static_get<75,4>();

    auto point = matrix.get(334,5);

    if(point.err)
    {
         log("Error " , point.err  );
    }


    //
    // if(point)
    // {
    //     log("Incorrect point ", point.err );
    // }

//    log( point )



    log("ala"," ma ", fmt::address<585938>() ," lat ");


    return 0;
}
