//
// Created by guozr on 23-12-20.
//

#include "config.h"
#include "log.h"
#include "util.h"

namespace sylar {
    template<class T, class FromStr, class ToStr>
    std::string ConfigVar<T, FromStr, ToStr>::toString() {
        try {
            RWMutexType::ReadLock lock(m_mutex);
            return ToStr()(m_val);
        } catch (std::exception &e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception "
                                              << e.what() << " convert: " << TypeToName<T>() << " to string"
                                              << " name=" << m_name;
        }
        return "";
    }

    template<class T, class FromStr, class ToStr>
    bool ConfigVar<T, FromStr, ToStr>::fromString(const std::string &val) {
        try {
            setValue(FromStr()(val));
        } catch (std::exception &e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception "
                                              << e.what() << " convert: string to " << TypeToName<T>()
                                              << " name=" << m_name
                                              << " - " << val;
        }
        return false;
    }


}