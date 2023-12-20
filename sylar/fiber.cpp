//
// Created by guozr on 23-12-20.
//

#include <atomic>
#include "fiber.h"

namespace sylar {

    static std::atomic<uint64_t> s_fiber_id{0};
    static std::atomic<uint64_t> s_fiber_count{0};

    Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller) : m_id(++s_fiber_id), m_cb(cb) {
        //TODO:
    }

    Fiber::~Fiber() {

    }

    void Fiber::reset(std::function<void()> cb) {

    }

    void Fiber::swapIn() {

    }

    void Fiber::swapOut() {

    }

    void Fiber::call() {

    }

    void Fiber::back() {

    }

    void Fiber::SetThis(Fiber *f) {

    }

    Fiber::ptr Fiber::GetThis() {
        return sylar::Fiber::ptr();
    }

    void Fiber::YieldToReady() {

    }

    void Fiber::YieldToHold() {

    }

    uint64_t Fiber::TotalFibers() {
        return 0;
    }

    void Fiber::MainFunc() {

    }

    void Fiber::CallerMainFunc() {

    }

    uint64_t Fiber::GetFiberId() {
        return 0;
    }

    Fiber::Fiber() {

    }
} // sylar