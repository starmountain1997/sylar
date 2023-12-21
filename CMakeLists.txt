cmake_minimum_required(VERSION 3.28)
project(sylar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(.)

add_subdirectory(third-party/yaml-cpp) # This is one way of referencing another

add_subdirectory(third-party/boost EXCLUDE_FROM_ALL)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(third-party/gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(sylar
        sylar/log.cpp
        sylar/log.h
        sylar/mutex.cpp
        sylar/mutex.h
        sylar/noncopyable.h
        sylar/singleton.h
        sylar/util.cpp
        sylar/util.h
        sylar/fiber.cpp
        sylar/fiber.h
        sylar/config.cpp
        sylar/config.h
        sylar/env.cpp
        sylar/env.h
        tests/test.cpp
        tests/test_config.cpp
)

target_link_libraries(
        sylar
        yaml-cpp
        Boost::lexical_cast
        GTest::gtest_main
)

include(GoogleTest)