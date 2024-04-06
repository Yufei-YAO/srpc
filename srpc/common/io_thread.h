#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include "common/sem.h"
#include "common/event_loop.h"

namespace srpc{


class IOThread{
public:
    typedef std::shared_ptr<IOThread> ptr;


    void start();
    void join();



    EventLoop::ptr getEventLoop() {
        return m_eventLoop;
    }

private:
    void Main();


private:
    std::thread m_thread;
    int32_t m_threadID;
    EventLoop::ptr m_eventLoop;
    Semaphore m_sem;

};




}