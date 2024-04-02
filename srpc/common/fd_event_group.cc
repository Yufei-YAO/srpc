#include <stdexcept>
#include "common/fd_event_group.h"
#include "common/log.h"


namespace srpc{


static FdEventGroup::ptr g_fdEventGroup = nullptr;


FdEventGroup::FdEventGroup(int size){
    m_fds.resize(size);
    for(size_t i = 0; i<m_fds.size(); ++i){
        m_fds[i].reset(new FdEvent(i));
    }
}
FdEvent::ptr FdEventGroup::getFdEvent(int fd){
    std::lock_guard<std::mutex> l(m_mutex);
    expand(fd); 
    return m_fds[fd];
}


void FdEventGroup::setFdEvent(FdEvent::ptr event){
    std::lock_guard<std::mutex> l(m_mutex);
    expand(event->getFd());
    m_fds[event->getFd()] = event;
}

void FdEventGroup::resetFdEvent(int fd){
    std::lock_guard<std::mutex> l(m_mutex);
    if(m_size <= fd){
        return;
    }
    m_fds[fd]->m_event = epoll_event();
}
void FdEventGroup::expand(int fd){
    if(fd<0){
        ERRORLOG("FdEventGroup::expand fail with fd=%d",fd);
        throw std::logic_error("fdevent expand with wrong fd");
    }
    if(m_size > fd){
        //DEBUGLOG("no need expand",fd);
        return;
    }
    //DEBUGLOG("need expand %d",fd);
    int old_size = m_fds.size();
    m_fds.resize(m_fds.size()*1.5);
    for(size_t i = old_size;i< m_fds.size();++i){
        m_fds[i].reset(new FdEvent(i));
    }
    m_size = m_fds.size();
}



FdEventGroup::ptr FdEventGroup::GetGlobalFdEventGroup(){
    if(g_fdEventGroup != nullptr){
        return g_fdEventGroup;
    }
    g_fdEventGroup.reset(new FdEventGroup(128));
    return g_fdEventGroup;


}



}