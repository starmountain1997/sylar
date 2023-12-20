//
// Created by guozr on 23-12-18.
//

#ifndef SYLAR_MUTEX_H
#define SYLAR_MUTEX_H

#include "noncopyable.h"
#include <pthread.h>

namespace sylar {
    template<class T>
    struct ScopedLockImpl {
    public:
        ScopedLockImpl(T &mutex) : m_mutex(mutex) {
            m_mutex.lock();
            m_locked = true;
        }

        ~ScopedLockImpl() { unlock(); }

        void lock() {
            if (!m_locked) {
                m_mutex.lock();
                m_locked = true;
            }
        }

        void unlock() {
            if (m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }

    private:
        T &m_mutex;
        bool m_locked;//是否上锁
    };

    // 自旋锁
    class Spinlock : Noncopyable {
    public:
        //局部锁
        typedef ScopedLockImpl<Spinlock> Lock;

        Spinlock() {
            pthread_spin_init(&m_mutex, 0);
        }

        ~Spinlock() { pthread_spin_destroy(&m_mutex); }

        void lock() { pthread_spin_lock(&m_mutex); }

        void unlock() { pthread_spin_unlock(&m_mutex); }

    private:
        pthread_spinlock_t m_mutex;
    };

} // sylar

#endif //SYLAR_MUTEX_H
