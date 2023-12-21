//
// Created by guozr on 23-12-18.
//

#ifndef SYLAR_UTIL_H
#define SYLAR_UTIL_H

#include <fstream>
#include <stdint.h>
#include <cxxabi.h>

namespace sylar {
    pid_t GetThreadId();

    uint32_t GetFiberId();

    template<class T>
    const char *TypeToName() {
        static const char *s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
        return s_name;
    }

    class FSUtil {

    public:
        static bool Mkdir(const std::string &dirname);

        static std::string Dirname(const std::string &filename);

        static bool OpenForWrite(std::ofstream &ofs, const std::string &filename, std::ios_base::openmode mode);
    };

} // sylar

#endif //SYLAR_UTIL_H
