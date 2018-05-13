#ifndef STREAMQUEUE_H
#define STREAMQUEUE_H
#include <krpc.hpp>
#include <krpc/stream.hpp>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "ThreadInterface.h"

template <class T>
class StreamQueue : public ThreadInterface {
private:
    std::deque<T>       streamQueue;
    krpc::Stream<T>     stream;
    std::mutex          queueMutex;

public:
    StreamQueue(krpc::Stream<T> _stream,
                float _rate = 50) {
        stream = _stream;
        stream.set_rate(_rate);
    }

    bool isEmpty() {
        return (streamQueue.size() == 0);
    }

    T receiveLast() {
        T val;
        queueMutex.lock();
        val = streamQueue.back();
        streamQueue.clear();
        queueMutex.unlock();
        return val;
    }

    T receive(){
        T val;
        queueMutex.lock();
        val = streamQueue.front();
        streamQueue.pop_front();
        queueMutex.unlock();
        return val;
    }

protected:
    void internalThreadEntry() {
        stream.acquire();
        while(isRunning()) {
            stream.wait();
            queueMutex.lock();
            streamQueue.push_back(stream());
            queueMutex.unlock();
        }
        stream.release();
        streamQueue.clear();
    }
};

#endif // STREAMQUEUE_H
