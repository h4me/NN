#include <iostream>
#include <string.h>
#include <algorithm>
#include <memory>
#include <iomanip>
#include <vector>
#include <list>
#include <stdlib.h>
#include <fstream>
namespace fmt {
     template<int V>
     struct address { enum { value = V };  };
}


double RandomDouble(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
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

template<class A, class B>
void esert_equal(A a , B b, const char *msg)
{
     if( ! (a == b) )
      log_err("ESERT : ", msg, " =====> " , a , "  == " , b, " <======= " );

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

      // Array tab;

       typedef std::shared_ptr<T> mem_pointer_t;
       mem_pointer_t ptr_dynamic;
       typedef std::shared_ptr<double> mem_pointer_weight_t;

       mem_pointer_weight_t ptr_weight;



       void init_dynamic()
       {
             ptr_dynamic = mem_pointer_t(new T[H*W],std::default_delete<T[]>());
             if(!ptr_dynamic)
             {
                 log("Error allocate memory");
             }

             ptr_weight=nullptr;
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
       double bias;
public:
       enum { value_H = H, value_W = W, poll_H = H/2, poll_W=W/2 };



      Matrix<poll_H,poll_W,double> getPolledMatrix()
      {

                 Matrix<poll_H,poll_W,double> matrix;

                 std::array<double,4> tab;

                 for(size_t i=0;i<H;i+=2)
                 {
                     for(size_t j=0;j<W;j+=2)
                     {
                         tab[0] = (*((Array*)(ptr_dynamic.get()))) [i][j];
                         tab[1] = (*((Array*)(ptr_dynamic.get()))) [i][j+1];
                         tab[2] = (*((Array*)(ptr_dynamic.get()))) [i+1][j];
                         tab[3] = (*((Array*)(ptr_dynamic.get()))) [i+1][j+1];
                         matrix.set(i/2,j/2) = *(std::max_element(tab.begin(), tab.end()));
                        // log(" >" , i/2, j/2 );
                     }


                 }



                 return matrix;

      }



       void initRandomValues()
       {
              auto b = begin();

              while(b!=end())
              {
                  *b = RandomDouble(-4.0, 4.0);
                  ++b;
              }
       }


       void show(const char* name)
       {
           std::cout << "=====> " << name << std::endl;
           for(size_t i=0;i<H;++i)
           {

               for(size_t j=0;j<W;++j)
               {
                 std::cout << "|" << std::setfill('0') << std::setw(sizeof(double)*2) << (*((Array*)(ptr_dynamic.get()))) [i][j];

               }
               std::cout << std::endl;

           }

       }

/*
       double CalculateWeightXMatrix()
       {
             double sum = 0x00;

             if(!ptr_weight)
             {
                 log("CalculateWeightXMatrix allocation");
                 return 0x00;
             }

            auto* w  = reinterpret_cast<double*>(ptr_weight.get());
            auto* m =  reinterpret_cast<T*>(ptr_dynamic.get());

            for(size_t i=0;i<H*W;++i)
            {
                sum +=  w[i] * static_cast<double>(m[i]);
            }

            log("CalculateWeightXMatrix : ", sum );
            return sum;
       }
*/
       void setBias(double _bias) { bias = _bias; }
       double getBias() { return  bias; }
/*
       void AllocateWeightSpace()
       {
           ptr_weight = mem_pointer_t(new double[H*W],std::default_delete<double[]>());
           if(!ptr_weight)
           {
               log("Error allocate memory ptr_weight");
           }

           memset(ptr_weight.get(),0,sizeof(double)*H*W);

           bias = 0x00;


           for(size_t index = 0;index < H*W ;++index)
           {
                   ((double*)(ptr_weight.get()))[index] = RandomDouble(-5.0,5.0);
           }

       }
    */
       Matrix() { init_dynamic(); /* memset(tab,0,sizeof(tab)); */  /* test_init_array(); */    }

       Matrix(const std::initializer_list<T>& l) : Matrix() {
                log("Initialize : ", l.size());
                std::transform(l.begin(),l.end(), begin(),[](T v){ return v; });
       }


       T* begin() { return    &(*((Array*)(ptr_dynamic.get())))[0][0];     }
       T* end()   { return    &(*(Array*)( ptr_dynamic.get()) )[H-1][W];   }

     //  Array* raw()  { return &tab; }
/*
       template<int x, int y>
       T static_get()
       {
           static_assert( (x < H && x >=0 ) , "Index array[X][..] out of range");
           static_assert( (y < W && y >= 0) , "Index array[...][Y] out of range");
           return tab[x][y];
       }
*/
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

    double Relu(double input)
    {
         log("Relu input ", input , " => " , ((input>0) ? input : 0) );
         return (input>0) ? input : 0;
    }

template<int IH, int IW,  int MH, int MW,int H = IH-MH+1 , int W=IW-MW+1>
Matrix<H,W , double> BuildPerceptronArray( Matrix<IH,IW>& input, Matrix<MH,MW,double>& mask)
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

                                  // Waga x Wartosc input a1*i1+a2*i2 .....

                                  total += static_cast<double>(point_input.value * point_mask.value);

                                //  log( total ," Calc input[", h+i, ",", w+j, "] x  mask[",h,",",w,"] = " , (int)point_input.value , " x ", (int)point_mask.value );


                           }
                    }

                    //auto c =  total / static_cast<double>(MH*MW) ;

                    //log("C ", c);

                    matrix.set(i,j) = Relu(  total +  mask.getBias() ) ;
                //    matrix.set(i,j) =  total / static_cast<double>(MH*MW);
                //    auto point_matrix = matrix.get(i,j);
                //    log("==>[",i,",",j,"] ", point_matrix.value);

               }

           }
        //  matrix.AllocateWeightSpace();
          return matrix;
}

/*
template<class VInput, class VMask>
void BuildLauyoutOfPerceptrones(VInput& v_input, VMask& v_mask)
{


       for(auto& single_matrix_input : v_input)
       {
                for(auto& single_matrix_mask : v_mask)
                {
                      BuildPerceptronArray(single_matrix_input,single_matrix_mask);
                }
       }

}
*/

template<int IH, int IW,  int MH, int MW,int H = IH-MH+1 , int W=IW-MW+1, class R=std::list< Matrix<H,W,double > > >
std::shared_ptr< R  > BuildLauyoutOfPerceptrones(std::vector<Matrix<IH,IW>>& v_input, std::vector<Matrix<MH,MW,double>>& v_mask)
{

       std::shared_ptr< R  > ptr( new R());

       if(!ptr)
       {
           log_err("can not allocate memory for Layout");
           return ptr;
       }

       for(auto& single_matrix_input : v_input)
       {
                for(auto& single_matrix_mask : v_mask)
                {
                     auto Input_X_Mask_matrix = BuildPerceptronArray(single_matrix_input,single_matrix_mask);

                     // 4x4 matrix

                     ptr->push_back(Input_X_Mask_matrix);
                }
       }

       return ptr;

}

void InitPseudoRandom()
{
    std::ifstream in("/dev/urandom", std::ios::in | std::ios::binary );

    if(!in)
    {
        log_err("Can not read file");
        return;
    }

    unsigned int RANDOM_VALUE=0x00;
    in.read( (char*)&RANDOM_VALUE,sizeof(RANDOM_VALUE));
    in.close();
    srand(RANDOM_VALUE);
}

//
// template<int H, int W, class CI>
// void CheckParity(Matrix<H,W,CI>& m)
// {
//     static_assert((H % 2 == 0) && (W % 2 == 0) , "We don't want pad matrix" );
// }


template<class TT>
void CheckParity()
{
    typedef typename std::remove_reference<TT>::type T;
    enum { H = T::value_H , W=  T::value_W };
    static_assert((H % 2 == 0) && (W % 2 == 0) , "We don't want pad matrix" );
}



int main(int argc, char **argv)
{

    InitPseudoRandom();

    log("------------------");

    std::vector<Matrix<6,6>> batch
    {

         { /* single input */
             1,1,1,0,0,0,
             1,1,1,0,0,0,
             1,1,1,0,0,0,
             0,0,0,0,0,0,
             0,0,0,0,0,0,
             0,0,0,0,0,0
            // 0,0,0,0,0,0

         },
         //
         // { /* single input */
         //     1,1,1,0,0,0,
         //     1,1,1,0,0,0,
         //     1,1,1,0,0,0,
         //     0,0,0,0,0,0,
         //     0,0,0,0,0,0,
         //     0,0,0,0,0,0,
         //     0,0,0,0,0,0
         //
         // }

    };

    std::vector<Matrix<3,3,double>> mask
    {
       {1,1,1,1,0,1,1,1,1},
       {1,1,1,0,0,0,0,0,0},
       {0,0,0,0,0,0,1,1,1}
    };

     Matrix<3,3,double> custom_mask;
     custom_mask.initRandomValues();
     mask.push_back(custom_mask);



auto PtrLayout = BuildLauyoutOfPerceptrones(batch, mask);
esert_equal(PtrLayout->size() , mask.size() * batch.size() ," Incorrect size of Layout ");

auto& layout_list = *PtrLayout;

CheckParity<decltype(*layout_list.begin())>();


for(auto& _matrix_layout : layout_list)
{
    // static_assert( (typename Matrix_t::value_H / 2) == 0, "Not parity H");

       _matrix_layout.show("layout 4x4");

      auto polled_matrix = _matrix_layout.getPolledMatrix();

      polled_matrix.show("polled");


    // int a = _matrix_layout;
//    auto value =  _matrix_layout.CalculateWeightXMatrix()  +  _matrix_layout.getBias();
//    value =  Relu(value);
}






/*
    Matrix<6,7> matrix {
      1,1,1,0,0,0,
      1,1,1,0,0,0,
      1,1,1,0,0,0,
      0,0,0,0,0,0,
      0,0,0,0,0,0,
      0,0,0,0,0,0,
      0,0,0,0,0,0
    };
*/




/*

    std::vector<Matrix<3,3>> mask {
              BuildNewMask_3x3(1,1,1,1,1,1,1,1,1),
              BuildNewMask_3x3(1,1,1,0,0,0,0,0,0),
              BuildNewMask_3x3(0,0,0,0,0,0,1,1,1)
           };
*/

//        for(auto& m : mask)
//           BuildPerceptronArray(matrix,m);
//
//
//
//      size_t cnt=0;
// //     matrix.for_each_pixel( [&cnt](unsigned char pixel){  std::cout << cnt << std::endl; cnt++; if(pixel) std::cout << "wrong" << std::endl;   });
//
//    // std::for_each(matrix.begin(),matrix.end(),[](unsigned char pixel){  std::cout << "pixel " << (int)pixel << std::endl;  });
//     //auto p =  matrix.static_get<75,4>();
//
//     matrix.set(2,4) = 1;
//     auto point = matrix.get(2,4);
//
//     if(point)
//     {
//          log_err( "Incorrect get" , point.err);
//     }
//     else
//     {
//         log("Point value=[" , (int)point.value, "]");
//     }
//
//
//
//
//     log("ala"," ma "," lat ");


    return 0;
}
