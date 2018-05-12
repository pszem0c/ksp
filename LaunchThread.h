
#ifndef LAUNCHTHREAD_H
#define LAUNCHTHREAD_H
#include "ThreadInterface.h"

#include <string>
#include vector


class LaunchThread : virtual public ThredInterface
{
public:

    // Constructors/Destructors
    //  


    /**
     * Empty Constructor
     */
    LaunchThread ();

    /**
     * Empty Destructor
     */
    virtual ~LaunchThread ();



    /**
     * @param  _vessel
     * @param  _rocketData
     */
     LaunchThread (krpc::services::SpaceCenter::Vessel _vessel, RocketData& _rocketData)
    {
    }


    /**
     */
    void internalThreadEntry ()
    {
    }

protected:

public:

protected:

public:

protected:


private:

    // Private attributes
    //  

    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData& rocketData;
public:

private:

public:


    // Private attribute accessor methods
    //  


    /**
     * Set the value of vessel
     * @param new_var the new value of vessel
     */
    void setVessel (krpc::services::SpaceCenter::Vessel new_var)     {
            vessel = new_var;
    }

    /**
     * Get the value of vessel
     * @return the value of vessel
     */
    krpc::services::SpaceCenter::Vessel getVessel ()     {
        return vessel;
    }

    /**
     * Set the value of flight
     * @param new_var the new value of flight
     */
    void setFlight (krpc::services::SpaceCenter::Flight new_var)     {
            flight = new_var;
    }

    /**
     * Get the value of flight
     * @return the value of flight
     */
    krpc::services::SpaceCenter::Flight getFlight ()     {
        return flight;
    }

    /**
     * Set the value of rocketData
     * @param new_var the new value of rocketData
     */
    void setRocketData (RocketData& new_var)     {
            rocketData = new_var;
    }

    /**
     * Get the value of rocketData
     * @return the value of rocketData
     */
    RocketData& getRocketData ()     {
        return rocketData;
    }
private:


    void initAttributes () ;

};

#endif // LAUNCHTHREAD_H
