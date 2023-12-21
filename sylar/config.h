//
// Created by guozr on 23-12-20.
//

#ifndef SYLAR_CONFIG_H
#define SYLAR_CONFIG_H

#include <memory>
#include <string>
#include <algorithm>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_set>
#include "mutex.h"

namespace sylar {
    class ConfigVarBase {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        explicit ConfigVarBase(std::string name, std::string description = "") : m_name(std::move(name)),
                                                                                 m_description(std::move(description)) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }

        virtual ~ConfigVarBase() = default;

        const std::string &getName() const { return m_name; }

        const std::string &getDescription() const { return m_description; }

        virtual std::string toString() = 0;

        virtual bool fromString(const std::string &val) = 0;

        virtual std::string getTypeName() const = 0;

    protected:
        /// 配置参数的名称
        std::string m_name;
        /// 配置参数的描述
        std::string m_description;
    };

    template<class F, class T>
    class LexicalCast {
    public:
        /**
         * @brief 类型转换
         * @param[in] v 源类型值
         * @return 返回v转换后的目标类型
         * @exception 当类型不可转换时抛出异常
         */
        T operator()(const F &v) {
            return boost::lexical_cast<T>(v);
        }
    };

    template<class T>
    class LexicalCast<std::string, std::vector<T> > {
    public:
        std::vector<T> operator()(const std::string &v) {
            YAML::Node node = YAML::Load(v);
            typename std::vector<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i) {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };

    template<class T>
    class LexicalCast<std::vector<T>, std::string> {
    public:
        std::string operator()(const std::vector<T> &v) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (auto &i: v) {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };


    template<class T, class FromStr=LexicalCast<std::string, T>, class ToStr=LexicalCast<T, std::string>>
    class ConfigVar : public ConfigVarBase {
    public:
        typedef RWMutex RWMutexType;
        typedef std::shared_ptr<ConfigVar> ptr;
        typedef std::function<void(const T &old_value, const T &new_value)> on_change_cb;

        ConfigVar(const std::string &name, const T &default_value, const std::string &description = "")
                : ConfigVarBase(name, description), m_val(default_value) {}

        std::string toString() override;

        bool fromString(const std::string &val) override;

        const T getValue() {
            RWMutexType::ReadLock lock(m_mutex);
            return m_val;
        }

        void setValue(const T &v) {
            {
                RWMutexType::ReadLock lock(m_mutex);
                if (v == m_val) {
                    return;
                }
                for (auto &i: m_cbs) {
                    i.second(m_val, v);
                }
            }
            RWMutexType::WriteLock lock(m_mutex);
            m_val = v;
        }


    private:
        RWMutexType m_mutex;
        T m_val;
        std::map<uint64_t, on_change_cb> m_cbs;
    };

    class Config {
    public:
        typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
        typedef RWMutex RWMutexType;


    private:
    };

}


#endif //SYLAR_CONFIG_H
