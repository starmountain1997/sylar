//
// Created by guozr on 23-12-14.
//

#include <memory>
#include <iostream>
#include <functional>
#include "log.h"
#include "util.h"
#include "yaml-cpp/yaml.h"
#include "stdarg.h"

namespace sylar {
    const char *loglevel_to_string(LogLevel level) {
        switch (level) {

            case LogLevel::UNKNOWN:
                return "UNKNOWN";
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARN:
                return "WARN";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::FATAL:
                return "FATAL";
            default:
                return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    LogLevel string_to_loglevel(const std::string &s) {
        if (s == "UNKNOWN")
            return LogLevel::UNKNOWN;
        else if (s == "DEBUG")
            return LogLevel::DEBUG;
        else if (s == "INFO")
            return LogLevel::INFO;
        else if (s == "WARN")
            return LogLevel::WARN;
        else if (s == "ERROR")
            return LogLevel::ERROR;
        else if (s == "FATAL")
            return LogLevel::FATAL;
        else
            return LogLevel::UNKNOWN;
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel level, const char *file, int32_t line, uint32_t elapse,
                       uint32_t thread_id, uint32_t fiber_id, uint64_t time, std::string thread_name) : m_file(
            file), m_line(line), m_elapse(elapse), m_threadId(thread_id), m_fiberId(fiber_id), m_time(time),
                                                                                                        m_threadName(
                                                                                                                std::move(
                                                                                                                        thread_name)),
                                                                                                        m_logger(
                                                                                                                std::move(
                                                                                                                        logger)),
                                                                                                        m_level(level) {

    }

    void LogEvent::format(const char *fmt, ...) {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    void LogEvent::format(const char *fmt, va_list al) {
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1) {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr e) : m_event(e) {}

    LogEventWrap::~LogEventWrap() { m_event->getLogger()->log(m_event->getLevel(), m_event); }

    std::stringstream &LogEventWrap::getSS() {
        return m_event->getSS();
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i: m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    std::ostream &
    LogFormatter::format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) {
        for (auto &i: m_items) {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }

    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << loglevel_to_string(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getThreadName();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
                : m_format(format) {
            if (m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            struct tm tm{};
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        FilenameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };


    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string &str)
                : m_string(str) {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
            os << "\t";
        }

    private:
        std::string m_string;
    };

    void LogFormatter::init() {
//str, format, type
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i) {
            if (m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while (n < m_pattern.size()) {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                                    && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        //std::cout << "*" << str << std::endl;
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                } else if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size()) {
                    if (str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0) {
                if (!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if (fmt_status == 1) {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
        static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

                XX(m, MessageFormatItem),           //m:消息
                XX(p, LevelFormatItem),             //p:日志级别
                XX(r, ElapseFormatItem),            //r:累计毫秒数
                XX(c, NameFormatItem),              //c:日志名称
                XX(t, ThreadIdFormatItem),          //t:线程id
                XX(n, NewLineFormatItem),           //n:换行
                XX(d, DateTimeFormatItem),          //d:时间
                XX(f, FilenameFormatItem),          //f:文件名
                XX(l, LineFormatItem),              //l:行号
                XX(T, TabFormatItem),               //T:Tab
                XX(F, FiberIdFormatItem),           //F:协程id
                XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
        };

        for (auto &i: vec) {
            if (std::get<2>(i) == 0) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end()) {
                    m_items.push_back(
                            FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
        //std::cout << m_items.size() << std::endl;
    }

    Logger::Logger(std::string name) : m_name(std::move(name)), m_level(LogLevel::DEBUG) {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::log(LogLevel level, const LogEvent::ptr &event) {
        if (level >= m_level) {
            auto self = shared_from_this();
            MutexType::Lock lock(m_mutex);
            if (!m_appenders.empty()) {
                for (auto &i: m_appenders) {
                    i->log(self, level, event);
                }
            } else if (m_root) {
                m_root->log(level, event);
            }
        }
    }

    void Logger::addAppender(const LogAppender::ptr &appender) {
        MutexType::Lock lock(m_mutex);
        if (!appender->getFormatter()) {
            MutexType::Lock ll(appender->m_mutex);
            appender->m_formatter = m_formatter;
        }
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(const LogAppender::ptr &appender) {
        MutexType::Lock lock(m_mutex);
        for (auto it = m_appenders.begin();
             it != m_appenders.end(); ++it) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::clearAppenders() {
        MutexType::Lock lock(m_mutex);
        m_appenders.clear();
    }

    void Logger::setFormatter(LogFormatter::ptr val) {

    }

    void Logger::setFormatter(const std::string &val) {

    }

    LogFormatter::ptr Logger::getFormatter() {
        return sylar::LogFormatter::ptr();
    }

    std::string Logger::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["name"] = m_name;
        if (m_level != LogLevel::UNKNOWN) {
            node["level"] = loglevel_to_string(m_level);
        }
        if (m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        for (auto &i: m_appenders) {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }
        for (auto &i: m_appenders) {
            node["appenders"].push_back(YAML::Load(i->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }


    bool FileLogAppender::reopen() {
        MutexType::Lock lock(m_mutex);
        if (m_filestream) {
            m_filestream.close();
        }
        return FSUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
    }


    void LogAppender::setFormatter(LogFormatter::ptr val) {
        MutexType::Lock lock(m_mutex);
        m_formatter = val;
        if (m_formatter) {
            m_hasFormatter = true;
        } else {
            m_hasFormatter = false;
        }
    }

    LogFormatter::ptr LogAppender::getFormatter() {
        MutexType::Lock lock(m_mutex);
        return m_formatter;
    }

    void StdoutLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
        if (level >= m_level) {
            MutexType::Lock lock(m_mutex);
            m_formatter->format(std::cout, logger, level, event);
        }
    }

    std::string StdoutLogAppender::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "StdoutLogAppender";
        if (m_level != LogLevel::UNKNOWN) {
            node["level"] = loglevel_to_string(m_level);
        }
        if (m_hasFormatter && m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    FileLogAppender::FileLogAppender(std::string filename)
            : m_filename(std::move(filename)) {
        reopen();
    }

    void FileLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) {
        if (level >= m_level) {
            uint64_t now = event->getTime();
            if (now >= (m_lastTime + 3)) {
                reopen();
                m_lastTime = now;
            }
            MutexType::Lock lock(m_mutex);
            //if(!(m_filestream << m_formatter->format(logger, level, event))) {
            if (!m_formatter->format(m_filestream, logger, level, event)) {
                std::cout << "error" << std::endl;
            }
        }
    }

    std::string FileLogAppender::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "FileLogAppender";
        node["file"] = m_filename;
        if (m_level != LogLevel::UNKNOWN) {
            node["level"] = loglevel_to_string(m_level);
        }
        if (m_hasFormatter && m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }


    LoggerManager::LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

        m_loggers[m_root->m_name] = m_root;

        init();
    }

    void LoggerManager::init() {

    }

    Logger::ptr LoggerManager::getLogger(const std::string &name) {
        MutexType::Lock lock(m_mutex);
        auto it = m_loggers.find(name);
        if (it != m_loggers.end()) {
            return it->second;
        }

        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }


    std::string LoggerManager::toYamlString() {
        MutexType::Lock lock(m_mutex);
        YAML::Node node;
        for (auto &i: m_loggers) {
            node.push_back(YAML::Load(i.second->toYamlString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
} // sylar