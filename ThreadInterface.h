#ifndef THREDINTERFACE_H
#define THREDINTERFACE_H

#include <thread>
#include <condition_variable>

class ThreadInterface {    
    enum ThreadState {
        Created,
        Running,
        Stopped,
        Finished,
    };

private:
    std::thread* thread;
    ThreadState threadState;
    static void internalThreadEntryFunc (void* thisObj);

protected:
    virtual void internalThreadEntry() = 0;
    static std::mutex threadFinishedMutex;
    static std::condition_variable threadFinishedCondition;

public:
    ThreadInterface ();
    virtual ~ThreadInterface () {}

    void startThread ();
    void stopThread ();
    bool isRunning ();
    bool isFinished();
    void detachThread ();
    void waitForJoin ();
};

#endif // THREDINTERFACE_H

