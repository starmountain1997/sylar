//
// Created by guozr on 23-12-14.
//

#ifndef SYLAR_LOG_H
#define SYLAR_LOG_H

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <fstream>
#include <map>
#include "mutex.h"
#include "singleton.h"

namespace sylar {

    class Logger;

    class LoggerManager;

    enum LogLevel {
        UNKNOWN, DEBUG, INFO, WARN, ERROR, FATAL
    };

    const char *loglevel_to_string(LogLevel level);

    static LogLevel string_to_loglevel(const std::string &s);

    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(std::shared_ptr<Logger> logger, LogLevel level, const char *file, int32_t line, uint32_t elapse,
                 uint32_t thread_id, uint32_t fiber_id, uint64_t time, std::string thread_name);


        const char *getFile() const { return m_file; }

        int32_t getLine() const { return m_line; }


        uint32_t getElapse() const { return m_elapse; }


        uint32_t getThreadId() const { return m_threadId; }


        uint32_t getFiberId() const { return m_fiberId; }


        uint64_t getTime() const { return m_time; }


        const std::string &getThreadName() const { return m_threadName; }


        std::string getContent() const { return m_ss.str(); }


        std::shared_ptr<Logger> getLogger() const { return m_logger; }


        LogLevel getLevel() const { return m_level; }


        std::stringstream &getSS() { return m_ss; }


        void format(const char *fmt, ...);


        void format(const char *fmt, va_list al);

    private:
        /// 文件名
        const char *m_file = nullptr;
        /// 行号
        int32_t m_line = 0;
        /// 程序启动开始到现在的毫秒数
        uint32_t m_elapse = 0;
        /// 线程ID
        uint32_t m_threadId = 0;
        /// 协程ID
        uint32_t m_fiberId = 0;
        /// 时间戳
        uint64_t m_time = 0;
        /// 线程名称
        std::string m_threadName;
        /// 日志内容流
        std::stringstream m_ss;
        /// 日志器
        std::shared_ptr<Logger> m_logger;
        /// 日志等级
        LogLevel m_level;
    };

    class LogEventWrap {
    public:
        LogEventWrap(LogEvent::ptr e);

        ~LogEventWrap();

        LogEvent::ptr getEvent() const { return m_event; }

        std::stringstream &getSS();

    private:
        LogEvent::ptr m_event;
    };

    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(const std::string &pattern);

        std::string format(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event);

        std::ostream &
        format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event);

        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;

            /**
             * @brief 析构函数
             */
            virtual ~FormatItem() {}

            /**
             * @brief 格式化日志到流
             * @param[in, out] os 日志输出流
             * @param[in] logger 日志器
             * @param[in] level 日志等级
             * @param[in] event 日志事件
             */
            virtual void
            format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;
        };

        /**
            * @brief 初始化,解析日志模板
            */
        void init();

        /**
         * @brief 是否有错误
         */
        bool isError() const { return m_error; }

        /**
         * @brief 返回日志模板
         */
        const std::string getPattern() const { return m_pattern; }

    private:
        /// 日志格式模板
        std::string m_pattern;
        /// 日志格式解析后格式
        std::vector<FormatItem::ptr> m_items;
        /// 是否有错误
        bool m_error = false;
    };

    class LogAppender {
        friend class Logger;

    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef Spinlock MutexType;

        virtual ~LogAppender() = default;

        virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;

        virtual std::string toYamlString() = 0;

        void setFormatter(LogFormatter::ptr val);

        LogFormatter::ptr getFormatter();

        LogLevel getLevel() const { return m_level; }

        void setLevel(LogLevel val) { m_level = val; }

    protected:
        /// 日志级别
        LogLevel m_level = LogLevel::DEBUG;
        /// 是否有自己的日志格式器
        bool m_hasFormatter = false;
        /// Mutex
        MutexType m_mutex;
        /// 日志格式器
        LogFormatter::ptr m_formatter;
    };

    class Logger : public std::enable_shared_from_this<Logger> {
        friend class LoggerManager;

    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef Spinlock MutexType;

        Logger(std::string name = "root");

        void log(LogLevel level, const LogEvent::ptr &event);

        void debug(LogEvent::ptr event) {
            log(LogLevel::DEBUG, event);
        }

        void info(LogEvent::ptr event) {
            log(LogLevel::INFO, event);

        }

        void warn(LogEvent::ptr event) {
            log(LogLevel::WARN, event);
        }

        void error(LogEvent::ptr event) {
            log(LogLevel::ERROR, event);
        }

        void fatal(LogEvent::ptr event) {
            log(LogLevel::FATAL, event);
        }

        void addAppender(const LogAppender::ptr &appender);

        void delAppender(const LogAppender::ptr &appender);

        void clearAppenders();

        LogLevel getLevel() const { return m_level; }

        void setLevel(LogLevel val) { m_level = val; }

        const std::string &getName() const { return m_name; }

        void setFormatter(LogFormatter::ptr val);

        void setFormatter(const std::string &val);

        LogFormatter::ptr getFormatter();

        std::string toYamlString();

    private:
        /// 日志名称
        std::string m_name;
        /// 日志级别
        LogLevel m_level;
        /// Mutex
        MutexType m_mutex;
        /// 日志目标集合
        std::list<LogAppender::ptr> m_appenders;
        /// 日志格式器
        LogFormatter::ptr m_formatter;
        /// 主日志器
        Logger::ptr m_root;
    };

    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;

        void log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) override;

        std::string toYamlString() override;

    };

    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;

        FileLogAppender(const std::string &filename);

        void log(Logger::ptr logger, LogLevel level, LogEvent::ptr event) override;

        std::string toYamlString() override;

        /**
         * @brief 重新打开日志文件
         * @return 成功返回true
         */
        bool reopen();

    private:
        /// 文件路径
        std::string m_filename;
        /// 文件流
        std::ofstream m_filestream;
        /// 上次重新打开时间
        uint64_t m_lastTime = 0;
    };

    class LoggerManager {
    public:
        typedef Spinlock MutexType;

        LoggerManager();

        Logger::ptr getLogger(const std::string &name);

        void init();

        Logger::ptr getRoot() const { return m_root; }

        std::string toYamlString();

    private:
        MutexType m_mutex;
        std::map<std::string, Logger::ptr> m_loggers;
        Logger::ptr m_root;
    };

    typedef sylar::Singleton<LoggerManager> LoggerMgr;
} // sylar

#endif //SYLAR_LOG_H
