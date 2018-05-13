#include "LaunchThread.h"
#include "DisplayThread.h"
#include "RocketData.h"
#include <string>
#include <stack>
#include <chrono>

LaunchThread::LaunchThread (krpc::services::SpaceCenter::Vessel _vessel, RocketData* _rocketData) {
    vessel = _vessel;
    flight = vessel.flight();
    rocketData = _rocketData;
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
    rocketData->setFairing(vessel.parts().fairings().front());
    rocketData->setStageCount(stageCount);
}


void LaunchThread::internalThreadEntry() {
    double finalOrbit = rocketData->getRequestedOrbitAltitude();
    double angle;
    launchState = LaunchState::Countdown;
    initRocketData();
    DisplayThread::instance().sendMsg("Stage count: " + std::to_string(rocketData->getStageCount()), MsgType::String);
    DisplayThread::instance().sendMsg("LaunchThread started.", MsgType::String);

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
            vessel.control().set_throttle(1);
            vessel.control().activate_next_stage();
            launchState = LaunchState::StageOne;
            break;
        case LaunchState::StageOne:

            break;
        case LaunchState::Circularize:

            break;

        default:
            break;
        }
    }
}


