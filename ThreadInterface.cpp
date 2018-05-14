#include "ThreadInterface.h"

std::condition_variable ThreadInterface::threadFinishedCondition;
std::mutex ThreadInterface::threadFinishedMutex;

ThreadInterface::ThreadInterface (std::string str) {
    name = str;
    thread = nullptr;
    threadState = ThreadState::Created;
}


void ThreadInterface::internalThreadEntryFunc(void *thisObj) {
    ((ThreadInterface*) thisObj)->internalThreadEntry();
    std::unique_lock<std::mutex> lck(threadFinishedMutex);
    ((ThreadInterface*)thisObj)->threadState = ThreadState::Finished;
    threadFinishedCondition.notify_one();
}

void ThreadInterface::startThread() {
    thread = new std::thread(internalThreadEntryFunc, this);
    threadState = ThreadState::Running;
}

void ThreadInterface::stopThread() {
    threadState = ThreadState::Stopped;
}

bool ThreadInterface::isRunning() {
    return (threadState == ThreadState::Running);
}

bool ThreadInterface::isFinished() {
    return (threadState == ThreadState::Finished);
}

void ThreadInterface::detachThread() {
    thread->detach();
}

void ThreadInterface::waitForJoin() {
    thread->join();
}

std::string ThreadInterface::getName() {
    return name;
}
