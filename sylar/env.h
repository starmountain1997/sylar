//
// Created by guozr on 23-12-21.
//

#ifndef SYLAR_ENV_H
#define SYLAR_ENV_H

#include "mutex.h"
#include "singleton.h"
#include <string>
#include <map>
#include <vector>

namespace sylar {

    class Env {
    public:
        typedef RWMutex RWMutexType;

        bool init(int argc, char **argv);

        void add(const std::string &key, const std::string &val);

        bool has(const std::string &key);

        void del(const std::string &key);

    private:

        RWMutexType m_mutex;
        std::map<std::string, std::string> m_args;
        std::vector<std::pair<std::string, std::string> > m_helps;
    };

    typedef sylar::Singleton<Env> ENvMgr;
} // sylar

#endif //SYLAR_ENV_H
