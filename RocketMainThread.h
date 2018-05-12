#ifndef ROCKETMAINTHREAD_H
#define ROCKETMAINTHREAD_H
#include "ThreadInterface.h"
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>

class RocketData;

class RocketMainThread : public ThreadInterface {
private:
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData* rocketData;
    std::vector<ThreadInterface* > activeThreads;
    std::mutex activeThreadsMutex;

public:
    RocketMainThread();
    virtual ~RocketMainThread ();

    void setVessel (krpc::services::SpaceCenter::Vessel _vessel);
    krpc::services::SpaceCenter::Vessel getVessel ();
    void launchToOrbit(double _orbitAltitude);

protected:
    void internalThreadEntry();
};

#endif // ROCKETMAINTHREAD_H
