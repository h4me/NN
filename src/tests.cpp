

#include <gtest/gtest.h>


int multi(int a , int b )
{
     return a*b;
}


TEST(MultiTest, PositiveNos) {
    ASSERT_EQ(6, multi(2,3));
    ASSERT_EQ(8, multi(2,4));
}


int main(int argc, char **argv) {

//     multi(2,3);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
