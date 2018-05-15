#include <iostream>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <krpc/services/krpc.hpp>
#include <exception>

#include "RocketMainThread.h"
#include "DisplayThread.h"

using namespace std;


int main() {
  try {
    auto conn = krpc::connect("puffin", "192.168.1.100");
    krpc::services::KRPC krpcService(&conn);
    krpc::services::SpaceCenter sc(&conn);
    if (krpcService.current_game_scene() == krpc::services::KRPC::GameScene::flight) {
      if (krpcService.paused()) {
        krpcService.set_paused(false);
      }

      RocketMainThread puffin(conn, sc, sc.active_vessel());

      DisplayThread::instance().startThread();
      puffin.startThread();
      //puffin.launchToOrbit(80000);
      puffin.hover();
      puffin.waitForJoin();
    }
  } catch (exception& e) {
    cout << e.what() << endl;
  }
}
