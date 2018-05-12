
#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H
#include "ThreadInterface.h"
#include <deque>
#include <mutex>

class DisplayMsg;

class DisplayThread : virtual public ThreadInterface {
private:
    std::deque<DisplayMsg* > displayMsgQueue;
    std::mutex displayMsgQueueMutex;

public:
    DisplayThread ();
    ~DisplayThread ();

    void sendMsg (int data, int type);
    void sendMsg (double data, int type);

protected:
    void internalThreadEntry ();

};

#endif // DISPLAYTHREAD_H
