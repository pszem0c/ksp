
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
        Circularize,
        Stop,
    };

private:
    krpc::Client&                       connection;
    krpc::services::SpaceCenter&        spaceCenter;
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData& rocketData;
    LaunchState launchState;

    //stream
    StreamQueue<double>* altitude;
    StreamQueue<double>* apoapsis;
    StreamQueue<float>* dynamicPressure;
    StreamQueue<std::tuple<double, double, double> >* remainingBurn;

    //auxiliaryFunction
    void traversePartTree(std::function<void (krpc::services::SpaceCenter::Part& part, int depth)> fun);
    void countStages();
    void computeAvailableDeltaV();
    void writeMsg(std::string str);

    //lauchProcedures
    void initRocketData();
    void countdown();
    void liftoff();
    double computePitchAngle(double altitudeVal, double atmosphereDepth, double finalOrbit, double exponentation = 0.5);
    void stageOneThrustControll(double altitude, float dynamicPressure);
    void apoapsisReached();
    void computeCircularizeNode(krpc::services::SpaceCenter::Node& node, double& burnTime, double& startTime);
    void jettisonFairing(double altitudeVal, bool &jettisoned);
    void circularizeBurn(krpc::services::SpaceCenter::Node& node);

public:
    LaunchThread (krpc::Client& _connection,krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData);
    virtual ~LaunchThread ();

protected:
    void internalThreadEntry();

};

#endif // LAUNCHTHREAD_H
