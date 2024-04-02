#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include "common/fd_event.h"



namespace srpc{



class FdEventGroup{
public:
    typedef std::shared_ptr<FdEventGroup> ptr;

    FdEventGroup(int size );
    FdEvent::ptr getFdEvent(int fd);


    void setFdEvent(FdEvent::ptr);
    void resetFdEvent(int fd);

    void expand(int fd);
public:
    static FdEventGroup::ptr GetGlobalFdEventGroup();


private:
    std::vector<FdEvent::ptr> m_fds;
    std::mutex m_mutex;
    int m_size = 0;

};








}