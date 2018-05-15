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
  typedef std::tuple<double, double, double> vector3;
private:
    krpc::Client&                   connection;
    krpc::services::SpaceCenter&        spaceCenter;
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData& rocketData;
    HoverState hoverState;
    krpc::services::SpaceCenter::ReferenceFrame landingSiteReferenceFrame;

    StreamQueue<double>* velocity;
    StreamQueue<vector3>* positionRelLs;

    //auxiliaryFunctions
    void writeMsg(std::string str);
    void createLandingReferenceFrame();
    vector3 crosProduct(const vector3& u, const vector3& v);
    double dotProduct(const vector3& u, const vector3& v);
    double magnitude(const vector3& v);
    double angleBetweenVectors(const vector3& u, const vector3& v);
    template<class T> T clamp(const T& val, const T& lower, const T& upper) {
      return std::min(upper, std::max(val, lower));
    }
    void normalizeVector(vector3& v);

public:
  HoverThread(krpc::Client& _connection, krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData);
  ~HoverThread();
protected:
  void internalThreadEntry();

};

#endif // HOVERTHREAD_H
