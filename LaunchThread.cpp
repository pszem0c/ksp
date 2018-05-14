#include "LaunchThread.h"
#include "DisplayThread.h"
#include "RocketData.h"
#include <string>
#include <stack>
#include <chrono>
#include <math.h>
#include <algorithm>

LaunchThread::LaunchThread (krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData):
  ThreadInterface("Launch thread: " + _vessel.name()),
  spaceCenter(_spaceCenter),
  vessel(_vessel),
  flight(_vessel.flight(_vessel.surface_reference_frame())),
  rocketData(_rocketData),
  altitude(nullptr),
  apoapsis(nullptr),
  dynamicPressure(nullptr),
  remainingBurn(nullptr) {}

LaunchThread::~LaunchThread () {
}

void LaunchThread::traversePartTree(std::function<void (krpc::services::SpaceCenter::Part&, int)> fun) {
  auto root = vessel.parts().root();
  std::stack<std::pair<krpc::services::SpaceCenter::Part, int>> stack;
  stack.push(std::pair<krpc::services::SpaceCenter::Part, int>(root, 0));
  while (!stack.empty()) {
    auto part = stack.top().first;
    auto depth = stack.top().second;
    stack.pop();
    fun(part, depth);
    for (auto child : part.children())
      stack.push(std::pair<krpc::services::SpaceCenter::Part, int>(child, depth+1));
  }
}

void LaunchThread::countStages() {
  traversePartTree( [this](krpc::services::SpaceCenter::Part& part, int) -> void {
    if (rocketData.getStageCount() < part.stage()) {
      rocketData.setStageCount(part.stage());
    }
  });
}

void LaunchThread::computeAvailableDeltaV() {
  //TODO: computing deltaC for more stages
  double thrustSum = 0.0;
  double thrustIspRatioSum = 0.0;
  for (auto engine: vessel.parts().engines()) {
    thrustSum += engine.max_vacuum_thrust();
    thrustIspRatioSum += engine.max_vacuum_thrust()/engine.vacuum_specific_impulse();
  }
  double isp = thrustSum/thrustIspRatioSum;
  rocketData.setAvailableDeltaV(isp*9.82*log(vessel.mass()/vessel.dry_mass()));
}

void LaunchThread::writeMsg(std::string str) {
  DisplayThread::instance().sendMsg(str, MsgType::String);
}

void LaunchThread::initRocketData() {
  countStages();
  computeAvailableDeltaV();
}

void LaunchThread::countdown() {
  writeMsg("3...");
  sleepFor(1.0);
  writeMsg("2...");
  sleepFor(1.0);
  writeMsg("1...");
  sleepFor(1.0);
  writeMsg("Liftoff!");
}

void LaunchThread::liftoff() {
  vessel.auto_pilot().engage();
  vessel.auto_pilot().target_pitch_and_heading(90, 90);
  vessel.control().set_throttle(1);
  vessel.control().activate_next_stage();
  launchState = LaunchState::StageOne;
  writeMsg("LaunchState: StageOne.");
  altitude->startThread();
  apoapsis->startThread();
  dynamicPressure->startThread();
}

double LaunchThread::computePitchAngle(double altitudeVal,
                                     double atmosphereDepth,
                                     double finalOrbit,
                                     double exponentation) {
  return 90*(1 - pow((altitudeVal/(atmosphereDepth*0.9 + finalOrbit*0.1)),exponentation));
}

void LaunchThread::stageOneThrustControll(double altitude, float dynamicPressure) {
  if (altitude > 20000.0) {
      vessel.control().set_throttle(0.8);
    } else if (dynamicPressure > 15000.0) {
      double error = ((dynamicPressure-15000.0)/1000.0);
      error = (error>1.0) ? 1.0 : error;
      vessel.control().set_throttle(1 - 0.30*error);
    } else {
      vessel.control().set_throttle(1);
    }
}

void LaunchThread::apoapsisReached() {
  for (int i = 0; i < 20; i++) {
      vessel.control().set_throttle(0.8-i*0.04);
      sleepFor(0.01);
    }
  vessel.control().set_throttle(0);
  writeMsg("Apoapsis reached: " + std::to_string(vessel.orbit().apoapsis_altitude()) );
  launchState = LaunchState::Circularize;
  writeMsg("LaunchState: Circularize.");
  vessel.auto_pilot().disengage();
  vessel.control().set_sas(true);
  vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::prograde);
  vessel.control().set_rcs(true);
  apoapsis->stopThread();
  apoapsis->waitForJoin();
  dynamicPressure->stopThread();
  dynamicPressure->waitForJoin();
}

void LaunchThread::jettisonFairing(double altitudeVal, bool& jettisoned) {
  if (!jettisoned && (altitudeVal > 45000.0)) {
    writeMsg("Fairing jettisoned at: " + std::to_string(altitudeVal));
    vessel.control().activate_next_stage();
    jettisoned = true;
  }
}

void LaunchThread::circularizeBurn(krpc::services::SpaceCenter::Node &node) {
  if (remainingBurn == nullptr) {
    remainingBurn = new StreamQueue<std::tuple<double, double, double> >(node.remaining_burn_vector_stream(node.orbital_reference_frame()));
    remainingBurn->startThread();
    apoapsis->startThread();
  }
  if (vessel.control().rcs()) {
      vessel.control().set_rcs(false);
  }

  if (!remainingBurn->isEmpty() && !apoapsis->isEmpty()) {
    double thrust;
    double remainingBurnVal = std::get<1>(remainingBurn->receiveLast());
    double apoapsisVal = apoapsis->receiveLast();
    if (remainingBurnVal < 50.0) {
      thrust = 0.1;
      if (remainingBurnVal < 10.) {
        thrust = 0.02;
      }
      if (apoapsisVal >= rocketData.getRequestedOrbitAltitude()) {
        thrust = 0.0;
        launchState = LaunchState::Stop;
        remainingBurn->stopThread();
        remainingBurn->waitForJoin();
        apoapsis->stopThread();
        apoapsis->waitForJoin();
        writeMsg("LaunchState: Stop.");
      }
    } else {
      thrust = 1.0;
    }
    vessel.control().set_throttle(thrust);
  }
}

void LaunchThread::computeCircularizeNode(krpc::services::SpaceCenter::Node& node,
                                          double& burnTime,
                                          double& startTime) {
  double availableDeltaV = vessel.specific_impulse()*9.82*log(vessel.mass()/vessel.dry_mass());
  double mu = vessel.orbit().body().gravitational_parameter();
  double r = vessel.orbit().apoapsis();
  double a1 = vessel.orbit().semi_major_axis();
  double a2 = r;
  double v1 = sqrt(mu*((2.0/r)-(1.0/a1)));
  double v2 = sqrt(mu*((2.0/r)-(1.0/a2)));
  double deltaV = v2 - v1;

  node = vessel.control().add_node(spaceCenter.ut() + vessel.orbit().time_to_apoapsis(), deltaV, 0, 0);

  writeMsg("AvailableDeltaV: " + std::to_string(availableDeltaV) );
  writeMsg("NodeDeltaV: " + std::to_string(deltaV) );
  if (deltaV > availableDeltaV) {
      writeMsg("No dzisiaj w kosmos to nie dolecimy :(");
    }

  double thrust = vessel.available_thrust();
  double isp = vessel.specific_impulse() * 9.82;
  double m0 = vessel.mass();
  double m1 = m0 * exp(-deltaV/isp);
  double flowRate = thrust / isp;
  burnTime = (m0 - m1) / flowRate;
  startTime = node.ut() - (burnTime/2);

  writeMsg("T: " + std::to_string(thrust) );
  writeMsg("isp: " + std::to_string(isp) );
  writeMsg("m0: " + std::to_string(m0) );
  writeMsg("m1: " + std::to_string(m1) );
  writeMsg("flow: " + std::to_string(flowRate) );
  writeMsg("BurnTime: " + std::to_string(burnTime) );
}

void LaunchThread::internalThreadEntry() {
  //variables
  double pitchAngle;
  double circularizeBurnTime;
  double circularizeStartTime;
  krpc::services::SpaceCenter::Node circularizeNode;

  bool fairingJettisoned = false;
  double finalOrbit = rocketData.getRequestedOrbitAltitude();
  double atmosphereDepth = vessel.orbit().body().atmosphere_depth();
  launchState = LaunchState::Countdown;

  //define streams
  altitude = new StreamQueue<double>(flight.mean_altitude_stream());
  apoapsis = new StreamQueue<double>(vessel.orbit().apoapsis_altitude_stream());
  dynamicPressure = new StreamQueue<float>(flight.dynamic_pressure_stream());

  initRocketData();
  writeMsg("LaunchThread started.");
  writeMsg("LaunchState: Countdown.");
  writeMsg("DeltaV: " + std::to_string(rocketData.getAvailableDeltaV()));

  while(isRunning()) {
    switch (launchState) {
      case LaunchState::Countdown:
        countdown();
        liftoff();
        break;
      case LaunchState::StageOne:
        if (!(altitude->isEmpty() || apoapsis->isEmpty() || dynamicPressure->isEmpty())) {
            double altitudeVal = altitude->receiveLast();
            double apoapsisVal = apoapsis->receiveLast();
            double dynamicPressureVal = dynamicPressure->receiveLast();

            pitchAngle = computePitchAngle(altitudeVal, atmosphereDepth, finalOrbit);
            vessel.auto_pilot().target_pitch_and_heading(pitchAngle, 90);

            // slow down near maxQ
            stageOneThrustControll(altitudeVal, dynamicPressureVal);

            // apoapsis reached
            if (apoapsisVal > (atmosphereDepth*0.9 + finalOrbit*0.1)) {
              apoapsisReached();
              computeCircularizeNode(circularizeNode, circularizeBurnTime, circularizeStartTime);
            }
          }
        break;
      case LaunchState::Circularize:
        if (!altitude->isEmpty()) {
          double altitudeVal = altitude->receiveLast();
          //jettison fairing
          jettisonFairing(altitudeVal, fairingJettisoned);

          if (spaceCenter.ut() > circularizeStartTime) {
            circularizeBurn(circularizeNode);
          } else if (altitudeVal > atmosphereDepth) {
            if (vessel.control().sas_mode() != krpc::services::SpaceCenter::SASMode::maneuver) {
              vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::maneuver);
            }
          }
        }
        break;
      case LaunchState::Idle:
        sleepFor(0.01);
        break;
      case LaunchState::Stop:
        sleepFor(1);
        vessel.control().activate_next_stage();
        launchState = LaunchState::Idle;
        stopThread();
        break;
      default:
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        break;
    }
  }
}


