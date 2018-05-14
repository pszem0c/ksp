#ifndef THREDINTERFACE_H
#define THREDINTERFACE_H

#include <thread>
#include <condition_variable>
#include <string>

class ThreadInterface {    
  enum ThreadState {
    Created,
    Running,
    Stopped,
    Finished,
  };

private:
  std::string name;
  std::thread* thread;
  ThreadState threadState;
  static void internalThreadEntryFunc (void* thisObj);

protected:
  void sleepFor(double seconds);
  virtual void internalThreadEntry() = 0;
  static std::mutex threadFinishedMutex;
  static std::condition_variable threadFinishedCondition;


public:
  ThreadInterface (std::string str = "");
  virtual ~ThreadInterface () {}

  void startThread ();
  void stopThread ();
  bool isRunning ();
  bool isFinished();
  void detachThread ();
  void waitForJoin ();
  std::string getName();
};

#endif // THREDINTERFACE_H

