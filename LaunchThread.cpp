#include "LaunchThread.h"
#include "DisplayThread.h"
#include "RocketData.h"
#include <string>
#include <stack>
#include <chrono>
#include <math.h>

LaunchThread::LaunchThread (krpc::services::SpaceCenter& _spaceCenter, krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData):
    ThreadInterface("Launch thread: " + _vessel.name()),
    spaceCenter(_spaceCenter),
    vessel(_vessel),
    flight(_vessel.flight()),
    rocketData(_rocketData){

}

LaunchThread::~LaunchThread () {

}



void LaunchThread::initRocketData() {
    // calc stage count and search for fairing
    int stageCount = 0;
    auto root = vessel.parts().root();
    std::stack<std::pair<krpc::services::SpaceCenter::Part, int>> stack;
    stack.push(std::pair<krpc::services::SpaceCenter::Part, int>(root, 0));
    while (!stack.empty()) {
      auto part = stack.top().first;
      auto depth = stack.top().second;
      stack.pop();
      if (part.decouple_stage() > stageCount) {
          stageCount = part.decouple_stage();
      }
      for (auto child : part.children())
        stack.push(std::pair<krpc::services::SpaceCenter::Part, int>(child, depth+1));
    }
    rocketData.setStageCount(stageCount);
}


void LaunchThread::internalThreadEntry() {
    double finalOrbit = rocketData.getRequestedOrbitAltitude();
    double angle;
    double atmosphereDepth = vessel.orbit().body().atmosphere_depth();
    krpc::services::SpaceCenter::Node circularizeNode;
    double circularizeBurnTime;
    launchState = LaunchState::Countdown;
    altitude = new StreamQueue<double>(flight.mean_altitude_stream());
    apoapsis = new StreamQueue<double>(vessel.orbit().apoapsis_altitude_stream());
    dynamicPressure = new StreamQueue<float>(flight.dynamic_pressure_stream());

    initRocketData();
    DisplayThread::instance().sendMsg("Stage count: " + std::to_string(rocketData.getStageCount()), MsgType::String);
    DisplayThread::instance().sendMsg("LaunchThread started.", MsgType::String);
    DisplayThread::instance().sendMsg("AtmosphereDepth= " + std::to_string(atmosphereDepth), MsgType::String);
    DisplayThread::instance().sendMsg("FinalOrbit= " + std::to_string(finalOrbit), MsgType::String);

    while(isRunning()) {
        switch (launchState) {
        case LaunchState::Countdown:
            DisplayThread::instance().sendMsg("3...", MsgType::String);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            DisplayThread::instance().sendMsg("2...", MsgType::String);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            DisplayThread::instance().sendMsg("1...", MsgType::String);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            DisplayThread::instance().sendMsg("Launch!", MsgType::String);
            vessel.auto_pilot().engage();
            vessel.auto_pilot().target_pitch_and_heading(90, 90);
            vessel.control().set_throttle(1);
            vessel.control().activate_next_stage();
            launchState = LaunchState::StageOne;
            altitude->startThread();
            apoapsis->startThread();
            dynamicPressure->startThread();
            break;
        case LaunchState::StageOne:
            if (!(altitude->isEmpty() || apoapsis->isEmpty() || dynamicPressure->isEmpty())) {
                double altitudeVal = altitude->receiveLast();
                double apoapsisVal = apoapsis->receiveLast();
                double dynamicPressureVal = dynamicPressure->receiveLast();

                angle = 90*(1 - pow((altitudeVal/(atmosphereDepth*0.9 + finalOrbit*0.1)),0.5));
                vessel.auto_pilot().target_pitch_and_heading(angle, 90);

                if (altitudeVal > 20000.0) {
                    vessel.control().set_throttle(0.8);
                } else if (dynamicPressureVal > 15000.0) {
                    vessel.control().set_throttle(0.7);
                } else {
                    vessel.control().set_throttle(1);
                }

                if (apoapsisVal > (atmosphereDepth*0.9 + finalOrbit*0.1)) {
                    DisplayThread::instance().sendMsg("Apoapsis reached: " + std::to_string(apoapsisVal) , MsgType::String);
                    for (int i = 0; i < 80; i++) {
                        vessel.control().set_throttle(0.8-i*0.01);
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    vessel.control().set_throttle(0);
                    launchState = LaunchState::Circularize;
                    apoapsis->stopThread();
                    dynamicPressure->stopThread();

                    double availableDeltaV = vessel.specific_impulse()*9.82*log(vessel.mass()/vessel.dry_mass());
                    double mu = vessel.orbit().body().gravitational_parameter();
                    double r = vessel.orbit().apoapsis();
                    double a1 = vessel.orbit().semi_major_axis();
                    double a2 = r;
                    double v1 = sqrt(mu*((2.0/r)-(1.0/a1)));
                    double v2 = sqrt(mu*((2.0/r)-(1.0/a2)));
                    double deltaV = v2 - v1;

                    circularizeNode = vessel.control().add_node(spaceCenter.ut() + vessel.orbit().time_to_apoapsis(), deltaV, 0, 0);

                    DisplayThread::instance().sendMsg("AvailableDeltaV: " + std::to_string(availableDeltaV) , MsgType::String);
                    DisplayThread::instance().sendMsg("NodeDeltaV: " + std::to_string(deltaV) , MsgType::String);
                    if (deltaV > availableDeltaV) {
                        DisplayThread::instance().sendMsg("No dzisiaj w kosmos to nie dolecimy :(", MsgType::String);
                    }

                    double thrust = vessel.available_thrust();
                    double isp = vessel.specific_impulse() * 9.82;
                    double m0 = vessel.mass();
                    double m1 = m0 * exp(-deltaV/isp);
                    double flowRate = thrust / isp;
                    circularizeBurnTime = (m0 - m1) / flowRate;

                    DisplayThread::instance().sendMsg("T: " + std::to_string(thrust) , MsgType::String);
                    DisplayThread::instance().sendMsg("isp: " + std::to_string(isp) , MsgType::String);
                    DisplayThread::instance().sendMsg("m0: " + std::to_string(m0) , MsgType::String);
                    DisplayThread::instance().sendMsg("m1: " + std::to_string(m1) , MsgType::String);
                    DisplayThread::instance().sendMsg("flow: " + std::to_string(flowRate) , MsgType::String);
                    DisplayThread::instance().sendMsg("BurnTime: " + std::to_string(circularizeBurnTime) , MsgType::String);

                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            break;
        case LaunchState::Circularize:
            if (!altitude->isEmpty()) {
                double altitudeVal = altitude->receiveLast();
                if ((altitudeVal > 45000.0)) {
                    DisplayThread::instance().sendMsg("Fairing jettisoned at: " + std::to_string(altitudeVal), MsgType::String);
                    vessel.control().activate_next_stage();
                    altitude->stopThread();
                }
            }

            vessel.control().set_rcs(true);
            vessel.auto_pilot().set_reference_frame(circularizeNode.reference_frame());
            vessel.auto_pilot().set_target_direction(std::make_tuple(0.0,1.0,0.0));

            if ((vessel.orbit().time_to_apoapsis() - (circularizeBurnTime / 2.0)) < 0.0) {
                vessel.control().set_throttle(1.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((circularizeBurnTime)*1000.0)));
                vessel.control().set_throttle(0.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                vessel.control().activate_next_stage();
                vessel.auto_pilot().disengage();
                launchState = LaunchState::Idle;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            break;
        default:
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            break;
        }
    }
}


