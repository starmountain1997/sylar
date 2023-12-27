//
// Created by guozr on 12/26/23.
//

#ifndef SYLAR_THREAD_H
#define SYLAR_THREAD_H

#include "noncopyable.h"
#include <memory>
#include <functional>
#include <string>
#include "mutex.h"

namespace sylar {

    class Thread : Noncopyable {
    public:
        typedef std::shared_ptr<Thread> ptr;

        Thread(std::function<void()> cb, const std::string &name);

        ~Thread();

        pid_t getId() const { return m_id; }

        const std::string &getName() const { return m_name; }

        void join();

        static Thread *GetThis();

        static const std::string &GetName();

        static void setName(const std::string &name);

    private:
        static void *run(void *arg);

        pid_t m_id;
        pthread_t m_thread = 0;
        std::function<void()> m_cb;
        std::string m_name;
        Semaphore m_semaphore;

    };

} // sylar

#endif //SYLAR_THREAD_H
