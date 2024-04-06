#pragma once


#include <semaphore.h>

namespace srpc{



class Semaphore{

public:

    Semaphore()     {sem_init(&m_sem,0,0);}
    ~Semaphore()    {sem_destroy(&m_sem);}


    void wait()     {sem_wait(&m_sem);}
    void post()     {sem_post(&m_sem);}


private:
    sem_t m_sem;
};








}