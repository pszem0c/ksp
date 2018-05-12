
#ifndef ROCKETDATA_H
#define ROCKETDATA_H

#include <string>

class RocketData {
public:
    RocketData ();
    ~RocketData ();

private:
    int stageCount;
    double availableDeltaV;
    double requestedOrbitAltitude;

public:

    void setStageCount (int _stageCount);
    int getStageCount ();

    void setAvailableDeltaV (double _deltaV);
    double getAvailableDeltaV ();

    void setRequestedOrbitAltitude (double _altitude);
    double getRequestedOrbitAltitude ();
};

#endif // ROCKETDATA_H
