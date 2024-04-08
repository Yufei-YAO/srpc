#include "common/log.h"
#include "common/event_loop.h"
#include "common/io_thread_pool.h"




namespace srpc{







IOThreadPool::IOThreadPool(int num_workers){
    if(num_workers != 0){
        m_numWorkers = num_workers;
    }
    else m_numWorkers = std::thread::hardware_concurrency();
    if(m_numWorkers ==0){
        m_numWorkers = 1;
        INFOLOG("must have one workers ");

    }
    INFOLOG("num of workers = %d",m_numWorkers);
}

IOThreadPool::~IOThreadPool(){
    stop();
}

void IOThreadPool::start(){
    for(int i = 0 ; i < m_numWorkers ; ++i){
        m_workers.push_back(IOThread::ptr(new IOThread));
        m_workers.back()->start();
        INFOLOG("workers %d start ",i);
    }
}
void IOThreadPool::stop(){
    for(size_t i = 0 ;  i < m_workers.size() ; ++i){
        m_workers[i]->getEventLoop()->stop();
        m_workers[i]->join();
    }
}

EventLoop::ptr IOThreadPool::getEventLoop(){
    static std::atomic<uint64_t> cur {0};
    uint64_t tmp = ++cur;
    return m_workers[tmp%m_workers.size()]->getEventLoop();
}

}