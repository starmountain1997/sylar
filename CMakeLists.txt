cmake_minimum_required(VERSION 3.28)
project(sylar)

include_directories(.)
include_directories(${Boost_INCLUDE_DIR})
add_subdirectory(third-party/yaml-cpp) # This is one way of referencing another
add_subdirectory(third-party/boost EXCLUDE_FROM_ALL)


set(CMAKE_CXX_STANDARD 17)

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
)

target_link_libraries(
        sylar
        yaml-cpp
        Boost::lexical_cast
)