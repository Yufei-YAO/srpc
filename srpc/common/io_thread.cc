#include "io_thread.h"
#include "util.h"






namespace srpc{
void IOThread::start(){
    m_thread = std::move(std::thread(&IOThread::Main, this)); 
    m_sem.wait();
}
void IOThread::join(){
    m_thread.join();

}


void IOThread::Main(){
    m_eventLoop = EventLoop::GetCurrentEventLoop();
    m_threadID = getThreadID();
    m_sem.post();
    m_eventLoop->loop();
}
}