#ifndef MNIST_FORMAT
#define MNIST_FORMAT
#include <fstream>
#include <algorithm>

#define log(x) std::cout << x << std::endl;
#define log_err(x) std::cout << "[ERROR]" << std::endl;

typedef unsigned int U32;
typedef unsigned char BYTE;
#pragma pack()
typedef struct IMAGE_FILE_HEADER_STRUCT
{
    U32 magic;
    U32 count;
    U32 rows;
    U32 columns;

// } IMAGE_FILE_HEADER_T __attribute__((packed));


protected:
  void swap_filed(U32& v)
  {
        std::reverse( (BYTE*)&v, ((BYTE*)&v) + sizeof(U32)  );
  }


  void swap()
  {
      swap_filed(magic);
      swap_filed(count);
      swap_filed(rows);
      swap_filed(columns);
  }
public:

   U32 validate()
   {
        swap();
        if(magic==0x803 && count==60000 && rows==28 && columns==28)
           return 1;
         log_err("IMAGE_FILE is not ok ");
         return 0;
   }

 } IMAGE_FILE_HEADER_T;

std::ostream& operator<<(std::ostream& o , const IMAGE_FILE_HEADER_T& data)
{
    o << std::hex << " Magic {" << data.magic << "} Count={" << std::dec<< data.count << "} Rows={" << data.rows << "} Columns={" << data.columns << "}"  << std::endl;
    return o;
}


#endif
