#include "RocketData.h"

// Constructors/Destructors
//  

RocketData::RocketData () {
}

RocketData::~RocketData () { }

krpc::services::SpaceCenter::Fairing RocketData::getFairing() const {
    return fairing;
}

void RocketData::setFairing(krpc::services::SpaceCenter::Fairing _fairing) {
    fairing = _fairing;
}

void RocketData::setStageCount(int _stageCount) {
    stageCount = _stageCount;
}

int RocketData::getStageCount() {
    return stageCount;
}

void RocketData::setAvailableDeltaV(double _deltaV) {
    availableDeltaV = _deltaV;
}

double RocketData::getAvailableDeltaV() {
    return availableDeltaV;
}

void RocketData::setRequestedOrbitAltitude(double _altitude) {
    requestedOrbitAltitude = _altitude;
}

double RocketData::getRequestedOrbitAltitude() {
    return requestedOrbitAltitude;
}

