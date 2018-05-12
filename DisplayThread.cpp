#include "DisplayThread.h"
#include "DisplayMsg.h"

void DisplayThread::sendMsg(int data, int type) {
}

void DisplayThread::sendMsg(double data, int type) {
}

void DisplayThread::sendMsg(std::string data, int type) {
    std::string* str = new std::string(data);
    if (str != nullptr) {
        DisplayMsg* msg = new DisplayMsg(static_cast<void*>(str), type);
        if (msg != nullptr) {
            displayMsgQueueMutex.lock();
            displayMsgQueue.push_back(msg);
            displayMsgQueueMutex.unlock();
        }
    }
}

void DisplayThread::internalThreadEntry() {
}


