#include "ThreadInterface.h"

ThreadInterface::ThreadInterface () {
    threadRunning = false;
    thread = nullptr;
}


void ThreadInterface::internalThreadEntryFunc(void *thisObj) {
    ((ThreadInterface*) thisObj)->internalThreadEntry();
}

void ThreadInterface::startThread() {
    thread = new std::thread(internalThreadEntryFunc, this);
    threadRunning = true;
}

void ThreadInterface::stopThread() {
    threadRunning = false;
}

bool ThreadInterface::isRunning() {
    return threadRunning;
}

void ThreadInterface::detachThread() {
    thread->detach();
}

void ThreadInterface::waitForJoin() {
    thread->join();
}

