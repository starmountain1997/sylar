//
// Created by guozr on 23-12-18.
//

#ifndef SYLAR_NONCOPYABLE_H
#define SYLAR_NONCOPYABLE_H


namespace sylar {
    class Noncopyable {
    public:
        Noncopyable() = default;

        ~Noncopyable() = default;

        Noncopyable(const Noncopyable &) = delete;

        Noncopyable &operator=(const Noncopyable &) = delete;
    };
}


#endif //SYLAR_NONCOPYABLE_H
