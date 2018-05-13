
#ifndef LAUNCHTHREAD_H
#define LAUNCHTHREAD_H
#include "ThreadInterface.h"

#include <string>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <mutex>
#include <condition_variable>

class RocketData;

class LaunchThread : public ThreadInterface {
    enum LaunchState {
        Countdown,
        StageOne,
        Circularize
    };

private:
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData* rocketData;
    LaunchState launchState;
    std::mutex streamMutex;
    std::condition_variable streamConditional;

    void initRocketData();

public:
    LaunchThread (krpc::services::SpaceCenter::Vessel _vessel, RocketData* _rocketData);
    virtual ~LaunchThread ();

protected:
    void internalThreadEntry();

};

#endif // LAUNCHTHREAD_H
