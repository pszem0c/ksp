#include "RocketMainThread.h"
#include "RocketData.h"
#include "LaunchThread.h"
#include "DisplayThread.h"
#include "HoverThread.h"
#include <exception>
#include <string>

RocketMainThread::RocketMainThread(krpc::services::SpaceCenter &_sc) : ThreadInterface("RocketThread: " + _sc.active_vessel().name()),
    spaceCenter(_sc),
    vessel(_sc.active_vessel()),
    flight(_sc.active_vessel().flight(_sc.active_vessel().surface_reference_frame())),
    rocketData(RocketData()) {}

RocketMainThread::~RocketMainThread () {
    if (isRunning()) {
        for(auto thread : activeThreads) {
            thread->stopThread();
        }
        stopThread();
    } else {
        for(auto thread : activeThreads) {
            thread->stopThread();
            waitForJoin();
        }
    }
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
    rocketData.setRequestedOrbitAltitude(_orbitAltitude);
    activeThreadsMutex.lock();
    activeThreads.push_back(launchThread);
    activeThreadsMutex.unlock();
    launchThread->startThread();
}

void RocketMainThread::hover() {
    ThreadInterface* hoverThread = new HoverThread(spaceCenter, vessel, rocketData);
    if (hoverThread == nullptr) {
        throw std::runtime_error("LaunchToOrbit: new error");
    }
    activeThreadsMutex.lock();
    activeThreads.push_back(hoverThread);
    activeThreadsMutex.unlock();
    hoverThread->startThread();
}

void RocketMainThread::internalThreadEntry() {
    DisplayThread::instance().sendMsg("RocketMainThread started.", MsgType::String);
    while(isRunning()) {
        std::unique_lock<std::mutex> lck(threadFinishedMutex);
        if (activeThreads.size() ) {
            threadFinishedCondition.wait(lck);
            for (auto thread: activeThreads) {
                if (thread->isFinished()) {
                    DisplayThread::instance().sendMsg("RocketMainThread: " + thread->getName()+ " joined.", MsgType::String);
                    thread->waitForJoin();
                    delete thread;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
