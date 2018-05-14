#ifndef HOVERTHREAD_H
#define HOVERTHREAD_H
#include "ThreadInterface.h"
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <StreamQueue.h>

class RocketData;

class HoverThread : public ThreadInterface {
  enum HoverState {
    Idle,
    Ascend,
    Hover,
    Descend,
    Stop
  };
private:
    krpc::services::SpaceCenter&        spaceCenter;
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData& rocketData;
    HoverState hoverState;

    StreamQueue<float>* mass;

    //auxiliaryFunctions
    void writeMsg(std::string str);

public:
  HoverThread(krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData);
  ~HoverThread();
protected:
  void internalThreadEntry();

};

#endif // HOVERTHREAD_H
