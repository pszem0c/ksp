#include "HoverThread.h"
#include "DisplayThread.h"
#include <cmath>
#include <tuple>
#include <memory>
#include <algorithm>
#include "regulatorpi.h"
#include "signalgenerator.h"

constexpr double pi() { return std::atan(1)*4; }

void HoverThread::writeMsg(std::string str) {
  DisplayThread::instance().sendMsg(str, MsgType::String);
}

void HoverThread::createLandingReferenceFrame() {
  double landing_latitude = -0.097208;
  double landing_longitude = -74.557675;
  double landing_altitude = 72;

  auto landing_position = vessel.orbit().body().msl_position(
    landing_latitude, landing_longitude, vessel.orbit().body().reference_frame());
  auto q_long = std::make_tuple(
    0.0,
    sin(-landing_longitude * 0.5 * pi() / 180.0),
    0.0,
    cos(-landing_longitude * 0.5 * pi() / 180.0)
    );
  auto q_lat = std::make_tuple(
    0.0,
    0.0,
    sin(landing_latitude * 0.5 * pi() / 180.0),
    cos(landing_latitude * 0.5 * pi() / 180.0)
  );
  landingSiteReferenceFrame =
    krpc::services::SpaceCenter::ReferenceFrame::create_relative(
      connection,
      krpc::services::SpaceCenter::ReferenceFrame::create_relative(
        connection,
        krpc::services::SpaceCenter::ReferenceFrame::create_relative(
          connection,
          vessel.orbit().body().reference_frame(),
          landing_position,
          q_long),
        std::make_tuple(0, 0, 0),
        q_lat),
      std::make_tuple(landing_altitude, 0, 0));
}

HoverThread::vector3 HoverThread::crosProduct(const HoverThread::vector3 &u, const HoverThread::vector3 &v) {
  return std::make_tuple(
    std::get<1>(u)*std::get<2>(v) - std::get<2>(u)*std::get<1>(v),
    std::get<2>(u)*std::get<0>(v) - std::get<0>(u)*std::get<2>(v),
    std::get<0>(u)*std::get<1>(v) - std::get<1>(u)*std::get<0>(v));
}

double HoverThread::dotProduct(const HoverThread::vector3 &u, const HoverThread::vector3 &v) {
  return
    std::get<0>(u)*std::get<0>(v) +
    std::get<1>(u)*std::get<1>(v) +
    std::get<2>(u)*std::get<2>(v);
}

double HoverThread::magnitude(const HoverThread::vector3 &v) {
  return sqrt(dotProduct(v, v));
}

double HoverThread::angleBetweenVectors(const HoverThread::vector3 &u, const HoverThread::vector3 &v) {
  double dp = dotProduct(u, v);
  if (dp == 0)
    return 0;
  writeMsg("dp= " + std::to_string(dp));
  double um = magnitude(u);
  double vm = magnitude(v);
  return acos(dp / (um*vm));
}

void HoverThread::normalizeVector(HoverThread::vector3 &v) {
  double mag = magnitude(v);
  v = vector3 {std::get<0>(v)/mag, std::get<1>(v)/mag, std::get<2>(v)/mag};
}

HoverThread::HoverThread(krpc::Client &_connection, krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData) :
  ThreadInterface("Launch thread: " + _vessel.name()),
  connection(_connection),
  spaceCenter(_spaceCenter),
  vessel(_vessel),
  flight(_vessel.flight(_vessel.surface_reference_frame())),
  rocketData(_rocketData),
  velocity(nullptr),
  positionRelLs(nullptr) { }

HoverThread::~HoverThread() { }

void HoverThread::internalThreadEntry() {
  //variables
  double mu = vessel.orbit().body().gravitational_parameter();
  double r = vessel.orbit().body().equatorial_radius();
  RegulatorPI velocityRegulator(0.1, 10, 1);
  velocityRegulator.prepare();
  velocityRegulator.setSetPoint(std::shared_ptr<SignalGenerator>(new SignalConstant));

  RegulatorPI pitchRegulator(0.01, 0, 1);
  pitchRegulator.prepare();
  pitchRegulator.setSetPoint(std::shared_ptr<SignalGenerator>(new SignalConstant));
  RegulatorPI yawRegulator(0.01, 0, 1);
  yawRegulator.prepare();
  yawRegulator.setSetPoint(std::shared_ptr<SignalGenerator>(new SignalConstant));


  //definestreams
  velocity = new StreamQueue<double>(vessel.flight(vessel.orbit().body().reference_frame()).vertical_speed_stream());

  createLandingReferenceFrame();
  positionRelLs = new StreamQueue<vector3>(vessel.position_stream(landingSiteReferenceFrame));

  vessel.control().set_rcs(true);
  vessel.auto_pilot().engage();
  vessel.auto_pilot().set_target_pitch(90);
  hoverState = HoverState::Ascend;
  vessel.control().activate_next_stage();
  while(isRunning()) {
      switch (hoverState) {
        case HoverState::Ascend:
          vessel.control().set_throttle(1.0);
          sleepFor(10.0);
          velocity->startThread();
          positionRelLs->startThread();
          hoverState = HoverState::Hover;
          vessel.auto_pilot().disengage();
          break;
        case HoverState::Hover:
          if (!velocity->isEmpty()) {
            double velocityVal = velocity->receiveLast();
            double gravityForce = mu*vessel.mass()/pow((r+flight.mean_altitude()),2);
            double cv = velocityRegulator.simulate(velocityVal);
            double thrust = clamp((gravityForce)/vessel.max_thrust() + cv, 0.0, 1.0);
            vessel.control().set_throttle(thrust);
          }
          if (!positionRelLs->isEmpty()) {
            vector3 positionRelLsVal = positionRelLs->receiveLast();
            vector3 lsHorizonPosition {0.0, std::get<1>(positionRelLsVal), std::get<2>(positionRelLsVal)};
            vector3 vesselDown {0.0, 0.0, 1.0};
            vector3 surfaceVesselDown = spaceCenter.transform_direction(vesselDown, vessel.reference_frame(), vessel.surface_reference_frame());
            double angle = atan2(std::get<2>(surfaceVesselDown), std::get<1>(surfaceVesselDown));
            double zError = sin(angle)* magnitude(lsHorizonPosition); // pitch
            double xError = cos(angle)* magnitude(lsHorizonPosition); // yaw
            vessel.control().set_pitch(clamp(-pitchRegulator.simulate(zError), -1.0, 1.0));
            vessel.control().set_yaw(clamp(-yawRegulator.simulate(xError), -1.0, 1.0));
            writeMsg("angle: " + std::to_string(angle) +
                     " ze: " + std::to_string(zError) +
                     " xe: " + std::to_string(xError) +
                     " p: " + std::to_string(vessel.control().pitch()) +
                     " y: " + std::to_string(vessel.control().yaw()));
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

