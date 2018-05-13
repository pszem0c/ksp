
#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H
#include "ThreadInterface.h"
#include <deque>
#include <string>

class DisplayMsg;

enum MsgType {
    String,
};

class DisplayThread : virtual public ThreadInterface {
private:
    DisplayThread() {}
    std::deque<DisplayMsg* > displayMsgQueue;
    std::mutex displayMsgQueueMutex;
    std::condition_variable msgReceivedCondition;

    void processStringMsg(DisplayMsg* msg);

public:
    static DisplayThread& instance() {
        static DisplayThread inst;
        return inst;
    }
    ~DisplayThread () {}

    //void sendMsg(int data, int type);
    //void sendMsg(double data, int type);
    void sendMsg(std::string data, int type);

    DisplayThread(DisplayThread const&)     = delete;
    void operator =(DisplayThread const&)   = delete;

protected:
    void internalThreadEntry ();

};

#endif // DISPLAYTHREAD_H
