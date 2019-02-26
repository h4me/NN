#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <memory>
#include "mnist_format.hpp"

template<class T>
void generic_read(std::ifstream& file, T& data)
{
      if(!file)
      {
          log_err("File is not opened")
      }

     file.read(reinterpret_cast<char*>(&data),sizeof(data));

     if(!file)
        log_err("Can not read content");

}

template<class T>
void generic_read(std::ifstream& file, T* data, size_t size)
{
      if(!file)
      {
          log_err("File is not opened")
      }

     file.read(reinterpret_cast<char*>(data),size);

     if(!file)
        log_err("Can not read content");

}



struct Matrix_28_28
{
   BYTE buff[28*28];
   typedef BYTE ARRAY[28][28];
   ARRAY* cast() { return reinterpret_cast<ARRAY*>(&buff); }
};

std::ostream& operator<<(std::ostream& o ,  Matrix_28_28& data)
{
    o << std::endl;
    for(size_t i=0;i<28;++i)
    {
        for(size_t j=0;j<28;++j)
        {
            std::cout << "|" << std::setfill('0') << std::setw(3) << std::dec << (U32)((*data.cast()) [i][j]);
        }
        o << std::endl;
    }

    return o;
}


template<class T>
using Vector = std::vector<T>;


template<class THeader,class TRecordData>
U32 LoadData(const std::string& file_name,Vector<TRecordData>& v_matrix)
{

    std::ifstream file(file_name.c_str());
    if(!file)
    {
       log("Can not open file " << file_name);
       return 0;
    }

    THeader hd;
    generic_read(file, hd);
    if(!hd.validate())
    {
        log_err("Validation fails "<< file_name);
        return 0;
    }

    if(v_matrix.size() != hd.count)
    {
        log_err("Incorrect init vector " << file_name << " hd.count " << hd.count << " " << v_matrix.size());
        return 0;
    }

    for(size_t i=0;i<hd.count;++i)
    {
         generic_read(file, v_matrix[i]);
    }

    file.close();

    return 1;
}

template<size_t x, size_t y, class T>
struct StaticMatrix;

template<class THeader,U32 H, U32  W, class T>
U32 LoadData(const std::string& file_name,Vector< StaticMatrix<H,W,T> >& v_matrix)
{


    std::ifstream file(file_name.c_str());
    if(!file)
    {
       log("Can not open file " << file_name);
       return 0;
    }

    THeader hd;
    generic_read(file, hd);
    if(!hd.validate())
    {
        log_err("Validation fails "<< file_name);
        return 0;
    }

    if(H*W != hd.count)
    {
        log_err("Incorrect init vector " << file_name << " hd.count " << hd.count << " " << v_matrix.size());
        return 0;
    }

    for(size_t i=0;i<hd.count;++i)
    {
         generic_read(file, &v_matrix.get_and_set(0,0) , H*W*sizeof(T));
    }

    file.close();

    return 1;

}


template<class T>
using Shared = std::shared_ptr<T>;


template<class T>
class Matrix;

template<class T >
class Kernel;

template<class T>
Matrix<T> Conv(const Matrix<T>& m_in, const Kernel<T>& m_mask);

template<class T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& m);

template<class T>
class Matrix {
     U32 x;
     U32 y;
     Shared<T> buff;

public:
       U32 getSize() const { return x*y; }
       Matrix(U32 x, U32 y) : x(x), y(y)
       {
            buff = Shared<T>(new T[x*y], std::default_delete<T[]>());
            if(!buff)
            {
              log_err("Can not allocate memory for buff");
            }
       }

       const T& get(size_t _x, size_t _y) const
       {
               if(_x >= x || _y >= y)
               {
                   log_err("get(): Index out of range x{"<< _x << " " << x << "} y{" << _y << " " << y << "}" );
                   exit(1);
               }

          return  (*reinterpret_cast<T(*)[x][y]>(buff.get()))[_x][_y];
       }

       T& get_and_set(size_t _x, size_t _y)
       {
               if(_x >= x || _y >= y)
               {
                   log_err("get_and_set(): Index out of range");
                   exit(1);
               }

          return  (*reinterpret_cast<T(*)[x][y]>( buff.get() ))[_x][_y];
       }


       template<class W>
       friend Matrix<W> Conv(const Matrix<W>& m_in, const Kernel<W>& m_mask);
       template<class W>
       friend std::ostream& operator<<(std::ostream& out, const Matrix<W>& m);

       void test()
       {
          Matrix<T> m(6,6);

          m.get_and_set(5,5 ) = 2;

          for(size_t i=0;i<(m.x*m.y);++i)
          {
              std::cout << i << " " <<  *(reinterpret_cast<T*>(m.buff.get()) + i) << std::endl;
          }
      }


};

template<size_t x, size_t y, class T>
struct StaticMatrix : public Matrix<T> {
          enum { value_x = x, value_y = y };
          typedef T value_type;
          StaticMatrix(): Matrix<T>(x,y) {}
};



template<class T >
class Kernel : public Matrix<T> {
      typedef Matrix<T> Base;
      T bias;
  public:
      using Base::Base;

      T getBias() const { return bias; }

      template<class TT>
      Matrix<TT> Conv(const Matrix<TT>& m_in, const Kernel<TT>& m_mask);
};


double Relu(double input)
{
     return (input>0) ? input : 0;
}

template<class T>
Matrix<T> Conv(const Matrix<T>& m_in, const Kernel<T>& m_mask)
{

    Matrix<T> m_out(m_in.x - m_mask.x + 1, m_in.y - m_mask.y +1);


    for(size_t i=0;i<m_out.x;++i)
    {
        for(size_t j=0;j<m_out.y;++j)
        {

            T total = 0x00;

            for(size_t h=0;h<m_mask.x;++h)
             {
                    for(size_t w=0;w<m_mask.y;++w)
                    {
                            auto point_mask = m_mask.get(h,w);

                            auto point_input = m_in.get(h+i,w+j);

                            total += static_cast<double>(point_input * point_mask);

                    }
             }

             m_out.get_and_set(i,j) = Relu(  total +  m_mask.getBias() ) ;

        }

    }

    return m_out;
}

template<class T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& m)
{
    for(size_t i=0;i<m.x;++i)
    {
        for(size_t j=0;j<m.y;++j)
        {
            std::cout << "|" << std::setfill(' ') << std::setw(4) << std::dec << m.get(i,j);
        }
         std::cout << std::endl;
    }

    return out;
}




int main(int argc, char** argv)
{

     Kernel<double> kernel(3,3);
     Matrix<double> m(20,20);

    // m.test();

     //std::cout << m.get(2,2) << std::endl;

    //auto pppp = Conv(m,kernel);
    //log( pppp );

    //
    // Vector<StaticMatrix<28, 28, U32 >> train_images_static(IMAGE_FILE_HEADER_TRAIN_T::limit);
    //
    // if(! LoadData<IMAGE_FILE_HEADER_TRAIN_T>("./bin/mnist/train-images-idx3-ubyte.gz.raw",train_images_static) )
    // {
    //         log_err("Error load Image train file");
    //         return 1;
    // }



    Vector<Matrix_28_28> train_images(IMAGE_FILE_HEADER_TRAIN_T::limit);

    if(! LoadData<IMAGE_FILE_HEADER_TRAIN_T>("./bin/mnist/train-images-idx3-ubyte.gz.raw",train_images) )
    {
        log_err("Error load Image train file");
        return 1;
    }

    Vector<BYTE> train_labels(LABEL_FILE_HEADER_TRAIN_T::limit);

    if(! LoadData<LABEL_FILE_HEADER_TRAIN_T>("./bin/mnist/train-labels-idx1-ubyte.gz.raw",train_labels) )
    {
        log_err("Error load train labels ");
        return 1;
    }

/***************************************************/

      Vector<Matrix_28_28> test_images(IMAGE_FILE_HEADER_TEST_T::limit);

      if(! LoadData<IMAGE_FILE_HEADER_TEST_T>("./bin/mnist/t10k-images-idx3-ubyte.gz.raw",test_images) )
      {
          log_err("Error load Image test file");
          return 1;
      }

      log( test_images[0] );

      Vector<BYTE> test_labels(LABEL_FILE_HEADER_TEST_T::limit);

      if(! LoadData<LABEL_FILE_HEADER_TEST_T>("./bin/mnist/t10k-labels-idx1-ubyte.gz.raw",test_labels) )
      {
          log_err("Error load test labels ");
          return 1;
      }








    return 0;
}
