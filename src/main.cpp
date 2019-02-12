#include <iostream>
#include <string.h>
#include <algorithm>
#include <memory>
#include <vector>
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

template<class...T>
void log_err(T&&...args)
{
    std::cout << "[ERROR]: ";
    log_internal(std::cout, std::forward<T>(args)...);
    std::cout << std::endl;
}



template<class R>
struct Ret {

         const char* err;
         R value;

     Ret() : err(0) {}
     operator bool() const {  return err!=0; }
    // operator R() const { return *value; }
};



template<int H, int W, class T=unsigned char>
class Matrix {
protected:
       typedef T Array[H][W];
       typedef unsigned int U32;

       Array tab;

       typedef std::shared_ptr<T> mem_pointer_t;
       mem_pointer_t ptr_dynamic;

       void init_dynamic()
       {
             ptr_dynamic = mem_pointer_t(new T[H*W],std::default_delete<T[]>());
             if(!ptr_dynamic)
             {
                 log("Error allocate memory");
             }
       }


        template<class I, class F>
        void init_array(I b, I e, F f)
        {
               size_t index=0;
               while(b!=e)
               {
                   f(*b, index++);
                   ++b;
               }
        }

        void test_init_array()
        {
                  init_array(begin(),end(), [](T& v, size_t _distance){
                            v = _distance;
                  });
        }

public:


       Matrix() { init_dynamic(); /* memset(tab,0,sizeof(tab)); */  /* test_init_array(); */    }

       Matrix(const std::initializer_list<T>& l) : Matrix() {
                log("Initialize : ", l.size());
                std::transform(l.begin(),l.end(), begin(),[](T v){ return v; });
       }


       T* begin() { return    &(*((Array*)(ptr_dynamic.get())))[0][0];     }
       T* end()   { return    &(*(Array*)( ptr_dynamic.get()) )[H-1][W];   }

     //  Array* raw()  { return &tab; }

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
               log_err(r.err);
               return r;
           }

           r.value = (*((Array*)(ptr_dynamic.get()))) [x][y];
         //  r.value = tab[x][y];
           return r;
       }


        class Setter {
                T* elem;
        public:
                Setter(T* e=nullptr) : elem(e) {}
                Setter& operator=(T v) {
                     *elem = v;
                     return *this;
                }
        };

        Setter set(size_t x, size_t y)
        {
            if(!(x >=0 && y>=0 && x<H && y<W))
            {
                log_err("set(x,y) Index of out range");
                return Setter();
            }

        //    (*((Array*)(ptr_dynamic.get()))) [x][y] = v;

            return Setter(  &(*((Array*)(ptr_dynamic.get()))) [x][y]  );
        }

        void for_each_pixel(std::function<void(T)> f)
        {
              std::for_each(begin(), end(), f );
            // for(size_t i=0;i<H*W;++i) f( reinterpret_cast<T*>(tab)[i])  ;
        }

};

    template<class ... T>
    Matrix<3,3> BuildNewMask_3x3(T...args)
    {
          static_assert(3*3 == sizeof ... (T), " Error : matrix size must be 9");
          Matrix<3,3> matrix {(unsigned char)(args)...};

         return matrix;
    }

template<int IH, int IW,  int MH, int MW,int H = IH-MH+1 , int W=IW-MW+1>
Matrix<H,W , double> BuildPerceptron( Matrix<IH,IW>& input, Matrix<MH,MW>& mask)
{

         //  enum { H = IH-MH+1 , W=IW-MW+1  };
           log("BuildPerceptrone {", H, W , "}");
           Matrix<H,W , double> matrix;

           for(size_t i=0;i<H;++i)
           {
               for(size_t j=0;j<W;++j)
               {

                   double total = 0x00;

                   for(size_t h=0;h<MH;++h)
                    {
                           for(size_t w=0;w<MW;++w)
                           {
                                  auto point_mask = mask.get(h,w);

                                  if(point_mask)
                                  {
                                      log_err("Point mask " , h, w);
                                  }

                                  auto point_input = input.get(h+i,w+j);

                                  if(point_input)
                                  {
                                      log_err("Point input" , h+i, w+j );
                                  }

                                  total += static_cast<double>(point_input.value * point_mask.value);

                                //  log( total ," Calc input[", h+i, ",", w+j, "] x  mask[",h,",",w,"] = " , (int)point_input.value , " x ", (int)point_mask.value );


                           }
                    }

                    //auto c =  total / static_cast<double>(MH*MW) ;

                    //log("C ", c);

                    matrix.set(i,j) = total / static_cast<double>(MH*MW);
                //    auto point_matrix = matrix.get(i,j);
                //    log("==>[",i,",",j,"] ", point_matrix.value);

               }

           }

          return matrix;
}


// BuildPerceptrones()



int main(int argc, char **argv)
{

    log("------------------");

    Matrix<6,7> matrix {
      1,1,1,0,0,0,
      1,1,1,0,0,0,
      1,1,1,0,0,0,
      0,0,0,0,0,0,
      0,0,0,0,0,0,
      0,0,0,0,0,0,
      0,0,0,0,0,0
    };

    std::vector<Matrix<3,3>> mask {
              {1,1,1,1,1,1,1,1,1},
              {1,1,1,0,0,0,0,0,0},
              {0,0,0,0,0,0,1,1,1}
           };
/*

    std::vector<Matrix<3,3>> mask {
              BuildNewMask_3x3(1,1,1,1,1,1,1,1,1),
              BuildNewMask_3x3(1,1,1,0,0,0,0,0,0),
              BuildNewMask_3x3(0,0,0,0,0,0,1,1,1)
           };
*/

       for(auto& m : mask)
          BuildPerceptron(matrix,m);



     size_t cnt=0;
//     matrix.for_each_pixel( [&cnt](unsigned char pixel){  std::cout << cnt << std::endl; cnt++; if(pixel) std::cout << "wrong" << std::endl;   });

   // std::for_each(matrix.begin(),matrix.end(),[](unsigned char pixel){  std::cout << "pixel " << (int)pixel << std::endl;  });
    //auto p =  matrix.static_get<75,4>();

    matrix.set(2,4) = 1;
    auto point = matrix.get(2,4);

    if(point)
    {
         log_err( "Incorrect get" , point.err);
    }
    else
    {
        log("Point value=[" , (int)point.value, "]");
    }




    log("ala"," ma "," lat ");


    return 0;
}
