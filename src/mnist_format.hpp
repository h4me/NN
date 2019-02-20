#ifndef MNIST_FORMAT
#define MNIST_FORMAT
#include <fstream>
#include <algorithm>

#define log(x) std::cout << x << std::endl;
#define log_err(x) std::cout << "[ERROR]:" << x << std::endl;

typedef unsigned int U32;
typedef unsigned char BYTE;
#pragma pack()

template<class T>
void swap_field(T& Head)
{
   std::reverse( (BYTE*)&Head, ((BYTE*)&Head) + sizeof(Head) );
}

template<class F, class ... T>
void swap_field(F& Head, T&...args)
{
      std::reverse( (BYTE*)&Head, ((BYTE*)&Head) + sizeof(Head)  );
      swap_field(args...);
}

template<U32 Limit>
struct IMAGE_FILE_HEADER_STRUCT
{
   enum { limit = Limit };
    U32 magic;
    U32 count;
    U32 rows;
    U32 columns;

    U32 validate()
    {
         swap();
         if(magic==0x803 && count==Limit && rows==28 && columns==28)
          return 1;

          log_err("IMAGE_FILE is not ok ");
          return 0;
    }

protected:

   void swap()
   {
       swap_field(magic,count,rows,columns);
   }

 };

typedef IMAGE_FILE_HEADER_STRUCT<60000> IMAGE_FILE_HEADER_TRAIN_T;
typedef IMAGE_FILE_HEADER_STRUCT<10000> IMAGE_FILE_HEADER_TEST_T;



 template<U32 Limit>
 struct LABEL_FILE_HEADER_STRUCT
 {
     enum { limit = Limit };
     U32 magic;
     U32 count;

     U32 validate()
     {
          swap();
          if(magic==0x801 && count==Limit)
             return 1;
           log_err("LABEL_FILE is not ok ");
           return 0;
     }

   protected:
     void swap()
     {
         swap_field(magic,count);
     }

  };


  typedef LABEL_FILE_HEADER_STRUCT<60000> LABEL_FILE_HEADER_TRAIN_T;
  typedef LABEL_FILE_HEADER_STRUCT<10000> LABEL_FILE_HEADER_TEST_T;


template<U32 N>
std::ostream& operator<<(std::ostream& o , const IMAGE_FILE_HEADER_STRUCT<N>& data)
{
    o << std::hex << " Magic {" << data.magic << "} Count={" << std::dec<< data.count << "} Rows={" << data.rows << "} Columns={" << data.columns << "}"  << std::endl;
    return o;
}

template<U32 N>
std::ostream& operator<<(std::ostream& o , const LABEL_FILE_HEADER_STRUCT<N>& data)
{
    o << std::hex << " Magic {" << data.magic << "} Count={" << std::dec<< data.count << "}"  << std::endl;
    return o;
}



#endif
