#include "DisplayThread.h"
#include "DisplayMsg.h"
#include <iostream>

//void DisplayThread::sendMsg(int data, int type) {
//}

//void DisplayThread::sendMsg(double data, int type) {
//}

void DisplayThread::sendMsg(std::string data, int type) {
    std::string* str = new std::string(data);
    if (str != nullptr) {
        DisplayMsg* msg = new DisplayMsg(static_cast<void*>(str), type);
        if (msg != nullptr) {
            std::unique_lock<std::mutex> lck(displayMsgQueueMutex);
            displayMsgQueue.push_back(msg);
            msgReceivedCondition.notify_one();
        }
    }
}

void DisplayThread::processStringMsg(DisplayMsg *msg) {
    std::string* str = static_cast<std::string* >(msg->getData());
    std::cout << *str << std::endl;
    delete str;
    delete msg;
}


void DisplayThread::internalThreadEntry() {
    while (isRunning()) {
        std::unique_lock<std::mutex> lck(displayMsgQueueMutex);
        msgReceivedCondition.wait(lck);
        while(displayMsgQueue.size()) {
            DisplayMsg* msg = displayMsgQueue.front();
            displayMsgQueue.pop_front();
            switch (msg->getType()) {
            case MsgType::String:
                processStringMsg(msg);
                break;
            default:
                break;
            }
        }
    }

}


