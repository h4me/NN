#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "mnist_format.hpp"

template<class T>
void generic_read(std::ifstream& file, T& data)
{
      if(!file)
      {
          log_err("File is not opened";)
      }

     file.read(reinterpret_cast<char*>(&data),sizeof(T));

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

template<class THeader,class TRecordData>
U32 LoadData(const std::string& file_name,std::vector<TRecordData>& v_matrix)
{

 //    std::string file_name = "./bin/mnist/train-images-idx3-ubyte.gz.raw";
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
        log_err("Validation fails");
        return 0;
    }

    if(v_matrix.size() != hd.count)
    {
        log_err("Incorrect init vector");
        return 0;
    }

    for(size_t i=0;i<hd.count;++i)
    {
         generic_read(file, v_matrix[i]);
    }


    file.close();

//    log("HD " << hd );

    return 1;
}

int main(int argc, char** argv)
{



    std::vector<Matrix_28_28> v_matrix(60000);

    if(! LoadData<IMAGE_FILE_HEADER_T,Matrix_28_28>("./bin/mnist/train-images-idx3-ubyte.gz.raw",v_matrix) )
    {
        log_err("Error Image file");
        return 1;
    }

    log( v_matrix[0] );






     // std::string file_name = "./bin/mnist/train-images-idx3-ubyte.gz.raw";
     // std::ifstream file(file_name.c_str());
     // if(!file)
     // {
     //    log("Can not open file " << file_name);
     //    return 0;
     // }
     //
     // IMAGE_FILE_HEADER_T hd;
     // generic_read(file, hd);
     // if(!hd.validate())
     // {
     //     log_err("Validation fails");
     //     return 1;
     // }
     //
     // std::vector<Matrix_28_28> v_matrix(60000);
     //
     //
     // for(size_t i=0;i<hd.count;++i)
     // {
     //      generic_read(file, v_matrix[i]);
     // }
     //
     //
     // file.close();
     //
     //
     // log("HD " << hd );
     //
     // log( v_matrix[0]);

    return 0;
}
