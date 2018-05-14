#ifndef ROCKETMAINTHREAD_H
#define ROCKETMAINTHREAD_H
#include "ThreadInterface.h"
#include "RocketData.h"
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>

class RocketMainThread : public ThreadInterface {
private:
    krpc::services::SpaceCenter&            spaceCenter;
    krpc::services::SpaceCenter::Vessel     vessel;
    krpc::services::SpaceCenter::Flight     flight;
    RocketData                              rocketData;
    std::vector<ThreadInterface* >          activeThreads;
    std::mutex                              activeThreadsMutex;

public:
    RocketMainThread(krpc::services::SpaceCenter& _sc);
    virtual ~RocketMainThread ();

    void setVessel (krpc::services::SpaceCenter::Vessel _vessel);
    krpc::services::SpaceCenter::Vessel getVessel ();
    void launchToOrbit(double _orbitAltitude);

protected:
    void internalThreadEntry();
};

#endif // ROCKETMAINTHREAD_H
