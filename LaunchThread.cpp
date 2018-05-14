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
  rocketData(_rocketData) {}

LaunchThread::~LaunchThread () {}

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
  rocketData.setAvailableDeltaV(vessel.specific_impulse()*9.82*log(vessel.mass()/vessel.dry_mass()));
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
      double error = error((dynamicPressure-15000.0)/1000.0);
      error = (error>1.0) ? 1.0 : error;
      vessel.control().set_throttle(1 - 0.30());
    } else {
      vessel.control().set_throttle(1);
    }
}

void LaunchThread::apoapsisReached() {
  for (int i = 0; i < 80; i++) {
      vessel.control().set_throttle(0.8-i*0.01);
      sleepFor(0.01);
    }
  vessel.control().set_throttle(0);
  writeMsg("Apoapsis reached: " + std::to_string(apoapsisVal) );
  launchState = LaunchState::Circularize;
  vessel.auto_pilot().disengage();
  apoapsis->stopThread();
  dynamicPressure->stopThread();
}

void LaunchThread::jettisonFairing() {
  if (!altitude->isEmpty()) {
    double altitudeVal = altitude->receiveLast();
    if ((altitudeVal > 45000.0)) {
        writeMsg("Fairing jettisoned at: " + std::to_string(altitudeVal));
        vessel.control().activate_next_stage();
        altitude->stopThread();
      }
  }
}

void LaunchThread::computeCircularizeNode(krpc::services::SpaceCenter::Node node,
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

  double finalOrbit = rocketData.getRequestedOrbitAltitude();
  double atmosphereDepth = vessel.orbit().body().atmosphere_depth();
  launchState = LaunchState::Countdown;

  //define streams
  altitude = new StreamQueue<double>(flight.mean_altitude_stream());
  apoapsis = new StreamQueue<double>(vessel.orbit().apoapsis_altitude_stream());
  dynamicPressure = new StreamQueue<float>(flight.dynamic_pressure_stream());

  initRocketData();
  writeMsg("LaunchThread started.");
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
            vessel.auto_pilot().target_pitch_and_heading(pitch, heading);

            // slow down near maxQ
            stageOneThrustControll(altitudeVal, dynamicPressureVal);

            // apoapsis reached
            if (apoapsisVal > (atmosphereDepth*0.9 + finalOrbit*0.1)) {
              apoapsisReached();
              computeCircularizeNode(circularizeNode, circularizeBurnTime);
            }
          }
        break;
      case LaunchState::Circularize:

        //jettison fairing
        jettisonFairing();

        vessel.control().set_rcs(true);
        vessel.control().set_sas(true);
        vessel.control().set_sas_mode(krpc::services::SpaceCenter::SASMode::maneuver);

        if (spaceCenter.ut() > circularizeStartTime) {
            vessel.control().set_rcs(false);
            vessel.control().set_throttle(1.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((circularizeBurnTime)*1000.0)));
            vessel.control().set_throttle(0.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            vessel.control().activate_next_stage();
            vessel.auto_pilot().disengage();
            launchState = LaunchState::Stop;
          }

        sleepFor(0.01);
        break;
      case LaunchState::Idle:
        sleepFor(0.01);
        break;
      case LaunchState::Stop:
        break;
      default:
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        break;
    }
  }
}


