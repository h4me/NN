cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "clang++-3.6")
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(PROJECT_SOURCE_DIR ${PROJECT_BINARY_DIR}/src)
SET(PARTY_DIR ${PROJECT_BINARY_DIR}/3party)



LINK_DIRECTORIES(${PARTY_DIR}/google_test_16/lib/)
include_directories( ${PARTY_DIR}/google_test_16/include)

# include(External-GTest.cmake)


add_executable(tests_app src/tests.cpp)
target_link_libraries(tests_app gtest gmock_main gmock pthread )


add_custom_command(TARGET tests_app POST_BUILD
    COMMAND ${PROJECT_BINARY_DIR}/bin/tests_app
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running Tests check..."
)


add_executable(neuron ${PROJECT_SOURCE_DIR}/main.cpp)
add_executable(poc ${PROJECT_SOURCE_DIR}/poc.cpp)
add_executable(mnist_loader ${PROJECT_SOURCE_DIR}/mnist_loader.cpp)
