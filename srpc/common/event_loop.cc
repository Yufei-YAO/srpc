#include <cerrno>
#include <cstring>
#include <mutex>
#include "common/event_loop.h"
#include "common/util.h"
#include "common/log.h"
#include "common/fd_event_group.h"

namespace srpc{

static thread_local EventLoop::ptr t_eventloop = nullptr;


static int g_maxEpollWaitTime = 10000;
static int g_maxEpollWaitEvent = 64;

EventLoop::ptr EventLoop::GetCurrentEventLoop(){
    if(t_eventloop != nullptr){
        return t_eventloop;
    }
    t_eventloop.reset(new EventLoop());
    //INFOLOG("eloop epoll fd:%d",t_eventloop->m_epollFd);
    return t_eventloop;
}
EventLoop::EventLoop(){
    m_epollFd = epoll_create(10); 
    //FdEventGroup::GetGlobalFdEventGroup()->setFdEvent(FdEvent::ptr(new FdEvent(m_epollFd))); 
    //INFOLOG("epoll fd=%d",m_epollFd);
    if(m_epollFd == -1){
        ERRORLOG("EventLoop::EventLoop epoll_create fail with errno=%d errstr=%s",errno,strerror(errno));
        exit(-1);
    }
    m_threadID = getThreadID();
    initWakeupEvent();
    initTimerEvent();
    DEBUGLOG("wakeup fd%d",m_wakeFd);
    //t_eventloop.reset(this);
    DEBUGLOG("create eventloop");
}
EventLoop::~EventLoop(){
    //FdEventGroup::GetGlobalFdEventGroup()->resetFdEvent(m_epollFd);   
    FdEventGroup::GetGlobalFdEventGroup()->resetFdEvent(m_wakeFd);   
    FdEventGroup::GetGlobalFdEventGroup()->resetFdEvent(m_timeFd);   


}


void EventLoop::loop(){
    m_isLooping = true;
    epoll_event eps[g_maxEpollWaitEvent];
    m_isStop = false;
    while(!isStop()){

        DEBUGLOG("in loop");
        std::unique_lock<std::mutex> ul(m_mutex);
        std::queue<std::function<void()>> tmp;
        tmp.swap(m_pendingTasks);
        ul.unlock();
        while(!tmp.empty()){
            auto k = tmp.front();
            tmp.pop();
            k();
        } 
        
        int rt = epoll_wait(m_epollFd,eps,g_maxEpollWaitEvent,g_maxEpollWaitTime);
        if(rt == -1){
            ERRORLOG("EventLoop::loop epoll_wait m_epollFd=%d fails with errno=%d errstr=%s",m_epollFd,errno,strerror(errno));
            //return;
            continue;
        }
        DEBUGLOG("epoll wait return rt=%d",rt);
        for(int i=0;i<rt;++i){
            epoll_event & ep = eps[i];
            DEBUGLOG("epoll wait return %d",(int)ep.data.fd);
            FdEvent::ptr fp = FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(ep.data.fd);
            if(fp==nullptr){
                continue;
            }
            if(ep.events & EPOLLIN){
                DEBUGLOG("epoll wakeup %d in",(int)ep.data.fd);
                addTask(fp->getHandler(FdEvent::Event::EPOLL_IN));
            }else if(ep.events & EPOLLOUT){
                DEBUGLOG("epoll wakeup %d out",(int)ep.data.fd);
                addTask(fp->getHandler(FdEvent::Event::EPOLL_OUT));
            }else if(ep.events & EPOLLERR){
                DEBUGLOG("epoll wakeup %d err",(int)ep.data.fd);
                addTask(fp->getHandler(FdEvent::Event::EPOLL_ERROR));
            }
        }
    }
    m_isLooping = false;
    
}
void EventLoop::wakeup(){
    std::dynamic_pointer_cast<WakeupFdEvent>(FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_wakeFd))->wakeup(); 
}
void EventLoop::stop(){
    m_isStop = true;
}


void EventLoop::addEpollEvent(FdEvent::ptr fd){
    auto p = [fd,this](){
        //INFOLOG("EventLoop::addEpollEvent");
        if(fd == nullptr){

            ERRORLOG("EventLoop::addEpollEvent");
        }
        epoll_event ev = fd->getEpollEvent();
        //ERRORLOG("EventLoop::addEpollEvent");
        int op = EPOLL_CTL_MOD;
        if(m_listenfds.find(fd->getFd()) == m_listenfds.end()){
            op = EPOLL_CTL_ADD;
            
        }
        //ERRORLOG("EventLoop::addEpollEvent");
        int rt = epoll_ctl(this->m_epollFd,op,fd->getFd(),&ev);
        if (rt == -1) { 
            ERRORLOG("EventLoop::addEpollEvent epoll_ctl fails on fd=%d, errno=%d, error=%s", fd->getFd(),errno, strerror(errno)); 
        } 
        this->m_listenfds.insert(fd->getFd());
        //ERRORLOG("EventLoop::addEpollEvent");
        DEBUGLOG("EventLoop::addEpollEvent on fd=%d", fd->getFd());
    };
    if(isInLoopThread()){
        p();
    } else {
        addTask(p);
        wakeup();
    }
}
void EventLoop::deleteEpollEvent(FdEvent::ptr fd){
    auto p = [fd,this](){
        int op = EPOLL_CTL_DEL;
        if(m_listenfds.find(fd->getFd()) == m_listenfds.end()){
            return; 
        }
        int rt = epoll_ctl(this->m_epollFd,op,fd->getFd(),0);
        if (rt == -1) { 
            ERRORLOG("EventLoop::deleteEpollEvent epoll_ctl fails on fd=%d, errno=%d, error=%s", fd->getFd(),errno, strerror(errno)); 
        } 
        this->m_listenfds.erase(fd->getFd());
    };
    if(isInLoopThread()){
        p();
    } else {
        addTask(p);
    }


}
void EventLoop::addTimerEvent(TimeEvent::ptr fd){
    std::dynamic_pointer_cast<TimerFdEvent>(FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_timeFd))->addTimeEvent(fd); 
}
void EventLoop::deleteTimerEvent(TimeEvent::ptr fd){
    std::dynamic_pointer_cast<TimerFdEvent>(FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_timeFd))->cancelTimeEvent(fd); 

}

void EventLoop::addTask(std::function<void()> cb, bool is_wakeup ){
    std::unique_lock<std::mutex> ul(m_mutex);  
    m_pendingTasks.push(cb);
    ul.unlock();

    if(is_wakeup){
        wakeup();
    }
}

bool EventLoop::isInLoopThread() const{
    return m_threadID == getThreadID();
}

void EventLoop::initWakeupEvent(){
    WakeupFdEvent::ptr w(new WakeupFdEvent());

    m_wakeFd = w->getFd();
    FdEventGroup::GetGlobalFdEventGroup()->setFdEvent(w); 
    addEpollEvent(w);
}
void EventLoop::initTimerEvent(){
    TimerFdEvent::ptr w(new TimerFdEvent());
    m_timeFd = w->getFd();
    FdEventGroup::GetGlobalFdEventGroup()->setFdEvent(w); 
    addEpollEvent(w);
}

}


