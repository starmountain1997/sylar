cmake_minimum_required(VERSION 3.28)
project(sylar)

add_subdirectory(third-party/yaml-cpp) # This is one way of referencing another

set(CMAKE_CXX_STANDARD 17)

add_executable(sylar main.cpp
        sylar/log.cpp
        sylar/log.h
        sylar/mutex.cpp
        sylar/mutex.h
        sylar/noncopyable.h
        sylar/singleton.h
        sylar/util.cpp
        sylar/util.h)

target_link_libraries(
        sylar
        yaml-cpp
)