//
// Created by guozr on 23-12-18.
//

#ifndef SYLAR_UTIL_H
#define SYLAR_UTIL_H

#include <fstream>
#include <stdint.h>

namespace sylar {
    pid_t GetThreadId();

    uint32_t GetFiberId();

    class FSUtil {

    public:
        static bool Mkdir(const std::string &dirname);

        static std::string Dirname(const std::string &filename);

        static bool OpenForWrite(std::ofstream &ofs, const std::string &filename, std::ios_base::openmode mode);
    };

} // sylar

#endif //SYLAR_UTIL_H
