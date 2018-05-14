
#ifndef LAUNCHTHREAD_H
#define LAUNCHTHREAD_H
#include "ThreadInterface.h"

#include <string>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include "StreamQueue.h"

class RocketData;

class LaunchThread : public ThreadInterface {
    enum LaunchState {
        Idle,
        Countdown,
        StageOne,
        Circularize
    };

private:
    krpc::services::SpaceCenter&        spaceCenter;
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData& rocketData;
    LaunchState launchState;

    //stream
    StreamQueue<double>* altitude;
    StreamQueue<double>* apoapsis;
    StreamQueue<float>* dynamicPressure;

    //lauchProcedures


    void traversePartTree(std::function<void (krpc::services::SpaceCenter::Part part)> fun);
    void initRocketData();
public:
    LaunchThread (krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData);
    virtual ~LaunchThread ();

protected:
    void internalThreadEntry();

};

#endif // LAUNCHTHREAD_H
