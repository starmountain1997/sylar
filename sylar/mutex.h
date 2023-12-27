//
// Created by guozr on 23-12-18.
//

#ifndef SYLAR_MUTEX_H
#define SYLAR_MUTEX_H

#include "noncopyable.h"
#include <pthread.h>
#include <cstdint>
#include <semaphore.h>

namespace sylar {
    class Semaphore : Noncopyable {
    public:
        Semaphore(uint32_t count = 0);

        ~Semaphore();

        void wait();

        //
        void notify();

    private:
        sem_t m_semaphore;
    };

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

    template<class T>
    struct ReadScopedLockImpl {
    public:
        ReadScopedLockImpl(T &mutex) : m_mutex(mutex) {
            m_mutex.rdlock();
            m_locked = true;
        }

        ~ReadScopedLockImpl() { unlock(); }

        void lock() {
            if (!m_locked) {
                m_mutex.rdlock();//上读锁
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
        bool m_locked;
    };

    template<class T>
    struct WriteScopedLockImpl {
    public:
        WriteScopedLockImpl(T &mutex) : m_mutex(mutex) {
            m_mutex.wrlock();
            m_locked = true;
        }

        ~WriteScopedLockImpl() {
            unlock();
        }

        void lock() {
            if (!m_locked) {
                m_mutex.wrlock();//上写锁
                m_locked = true;
            }
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            if (m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }

    private:
        T &m_mutex;
        bool m_locked;
    };

    class RWMutex : Noncopyable {
    public:
        ///局部读锁
        typedef ReadScopedLockImpl<RWMutex> ReadLock;
        ///局部写锁
        typedef WriteScopedLockImpl<RWMutex> WriteLock;

        RWMutex() { pthread_rwlock_init(&m_lock, nullptr); }

        ~RWMutex() { pthread_rwlock_destroy(&m_lock); }

        void rdlock() { pthread_rwlock_rdlock(&m_lock); }

        void wrlock() { pthread_rwlock_wrlock(&m_lock); }

        void unlock() { pthread_rwlock_unlock(&m_lock); }

    private:
        pthread_rwlock_t m_lock;
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
