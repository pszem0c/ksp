#include <iostream>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <exception>

#include "RocketMainThread.h"
#include "DisplayThread.h"

using namespace std;


int main() {
    try {
        auto conn = krpc::connect("puffin", "192.168.1.100");
        krpc::services::SpaceCenter sc(&conn);

        RocketMainThread puffin;
        puffin.setVessel(sc.active_vessel());
        puffin.setSpaceCenter(&sc);
        DisplayThread::instance().startThread();


        puffin.startThread();
        puffin.launchToOrbit(80000);
        puffin.waitForJoin();
    } catch (exception& e) {
        cout << e.what() << endl;
    }
}
