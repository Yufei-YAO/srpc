#pragma once
#include <vector>
#include <atomic>
#include <memory>
#include "common/io_thread.h"


namespace srpc{

class IOThreadPool{
public:
    typedef std::shared_ptr<IOThreadPool> ptr;
    IOThreadPool(int num_workers = 0);
    ~IOThreadPool();

    void start();
    void stop();

    EventLoop::ptr getEventLoop();

private:
    std::vector<IOThread::ptr> m_workers;
    int m_numWorkers;
    
};




}