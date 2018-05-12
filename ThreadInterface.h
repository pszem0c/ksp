#ifndef THREDINTERFACE_H
#define THREDINTERFACE_H

#include <thread>

class ThreadInterface {
private:
    std::thread* thread;
    bool threadRunning;

    static void* internalThreadEntryFunc (void* thisObj);

protected:
    virtual void internalThreadEntry () = 0;

public:
    ThreadInterface ();
    virtual ~ThreadInterface () {}

    void startThread ();
    void stopThread ();
    bool isRunning ();
    void detachThread ();
    void waitForJoin ();
    bool isThreadRunning ();


};

#endif // THREDINTERFACE_H
