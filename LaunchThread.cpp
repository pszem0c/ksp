#include "LaunchThread.h"

LaunchThread::LaunchThread (krpc::services::SpaceCenter::Vessel _vessel, RocketData* _rocketData) {
    vessel = _vessel;
    flight = vessel.flight();
    rocketData = _rocketData;
}

LaunchThread::~LaunchThread () {

}

void LaunchThread::internalThreadEntry() {

}


