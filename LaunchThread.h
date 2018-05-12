
#ifndef LAUNCHTHREAD_H
#define LAUNCHTHREAD_H
#include "ThreadInterface.h"

#include <string>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>

class RocketData;

class LaunchThread : public ThreadInterface {
private:
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData* rocketData;

public:
    LaunchThread (krpc::services::SpaceCenter::Vessel _vessel, RocketData* _rocketData);
    virtual ~LaunchThread ();

protected:
    void internalThreadEntry();

};

#endif // LAUNCHTHREAD_H
