#ifndef STREAMQUEUE_H
#define STREAMQUEUE_H
#include <krpc.hpp>
#include <krpc/stream.hpp>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>

template <class T>
class StreamQueue {
private:
    std::deque<T>               streamQueue;
    krpc::Stream<T>             stream;
    std::mutex*                 streamDataReceivedMutex;
    std::condition_variable*    streamDataReceivedCondition;

public:
    StreamQueue(krpc::Stream<T> _stream, std::mutex* _streamDataReceivedMutex, std::condition_variable* _streamDataReceivedCondition = nullptr, float _rate = 50) {
        stream = _stream;
        streamDataReceivedMutex = _streamDataReceivedMutex;
        streamDataReceivedCondition = _streamDataReceivedCondition;
        stream.set_rate(_rate);
    }

    void startQueue() {
        stream.start();
    }

    bool isEmpty() {
        return (streamQueue.size() == 0);
    }

    T receive() {
        std::unique_lock<std::mutex> lck(&streamDataReceivedMutex);
        T result = streamQueue.front();
        streamQueue.pop_front();
        return result;
    }

};

#endif // STREAMQUEUE_H
