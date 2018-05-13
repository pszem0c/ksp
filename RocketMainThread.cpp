#include "RocketMainThread.h"
#include "RocketData.h"
#include "LaunchThread.h"
#include "DisplayThread.h"
#include <exception>
#include <string>

RocketMainThread::RocketMainThread () {
    rocketData = new RocketData();
}

RocketMainThread::~RocketMainThread () {
    if (isRunning()) {
        for(auto thread : activeThreads) {
            thread->stopThread();
            thread->waitForJoin();
        }
        stopThread();
    }
    delete rocketData;
}

void RocketMainThread::setSpaceCenter(krpc::services::SpaceCenter* _spaceCenter) {
    spaceCenter = _spaceCenter;
}

void RocketMainThread::setVessel(krpc::services::SpaceCenter::Vessel _vessel) {
    vessel = _vessel;
    flight = vessel.flight(vessel.surface_reference_frame());
}

krpc::services::SpaceCenter::Vessel RocketMainThread::getVessel() {
    return vessel;
}

void RocketMainThread::launchToOrbit(double _orbitAltitude) {
    ThreadInterface* launchThread = new LaunchThread(spaceCenter, vessel, rocketData);
    if (launchThread == nullptr) {
        throw std::runtime_error("LaunchToOrbit: new error");
    }
    rocketData->setRequestedOrbitAltitude(_orbitAltitude);
    activeThreadsMutex.lock();
    activeThreads.push_back(launchThread);
    activeThreadsMutex.unlock();
    launchThread->startThread();
}

void RocketMainThread::internalThreadEntry() {
    DisplayThread::instance().sendMsg("RocketMainThread started.", MsgType::String);
    while(isRunning()) {
        std::unique_lock<std::mutex> lck(threadFinishedMutex);
        threadFinishedCondition.wait(lck);
        for (auto thread: activeThreads) {
            if (thread->isFinished()) {
                DisplayThread::instance().sendMsg("RocketMainThread: thread joined.", MsgType::String);
                thread->waitForJoin();
            }
        }
    }
}
