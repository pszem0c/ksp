#include "RocketData.h"

// Constructors/Destructors
//  

RocketData::RocketData() :
    stageCount(0), availableDeltaV(0), requestedOrbitAltitude(0) {
}

RocketData::~RocketData () { }

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

