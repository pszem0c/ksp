#include "HoverThread.h"
#include "DisplayThread.h"
#include <cmath>

void HoverThread::writeMsg(std::string str) {
  DisplayThread::instance().sendMsg(str, MsgType::String);
}

HoverThread::HoverThread(krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData) :
  ThreadInterface("Launch thread: " + _vessel.name()),
  spaceCenter(_spaceCenter),
  vessel(_vessel),
  flight(_vessel.flight(_vessel.surface_reference_frame())),
  rocketData(_rocketData),
  mass(nullptr) { }

HoverThread::~HoverThread() { }

void HoverThread::internalThreadEntry() {
  //variables
  double mu = vessel.orbit().body().gravitational_parameter();
  double r = vessel.orbit().body().equatorial_radius();
  //definestreams
  mass = new StreamQueue<float>(vessel.mass_stream());
  vessel.control().set_rcs(true);
  vessel.auto_pilot().engage();;
  vessel.auto_pilot().target_pitch_and_heading(90, 90);
  hoverState = HoverState::Ascend;
  vessel.control().activate_next_stage();
  while(isRunning()) {
      switch (hoverState) {
        case HoverState::Ascend:
          vessel.control().set_throttle(1.0);
          sleepFor(10.0);
          mass->startThread();
          hoverState = HoverState::Hover;
          break;
        case HoverState::Hover:
          if (!mass->isEmpty()) {
            double massVal = mass->receiveLast();
            double gravityForce = mu*massVal/pow((r+flight.mean_altitude()),2);
            double velocity = vessel.flight(vessel.orbit().body().reference_frame()).vertical_speed();
            writeMsg("v= " + std::to_string(velocity));
            double thrust = gravityForce/vessel.max_thrust() - 0.1*velocity;
            vessel.control().set_throttle(thrust);
          }
          break;
        case HoverState::Descend:
          break;
        case HoverState::Stop:
          break;
        case HoverState::Idle:
          break;
        default:
          break;
        }
  }
}

