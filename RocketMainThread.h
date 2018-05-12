
#ifndef ROCKETMAINTHREAD_H
#define ROCKETMAINTHREAD_H
#include "ThreadInterface.h"
#include "ThreadInterface.h"

#include <string>
#include vector


class RocketMainThread : virtual public ThredInterface, virtual public ThredInterface
{
public:

    // Constructors/Destructors
    //  


    /**
     * Empty Constructor
     */
    RocketMainThread ();

    /**
     * Empty Destructor
     */
    virtual ~RocketMainThread ();



    /**
     * @param  _vessel
     * @param  _rocketData
     */
     RocketMainThread (krcp::services::SpaceCenter::Vessel _vessel, RocketData _rocketData)
    {
    }

protected:

public:

protected:

public:

protected:



    /**
     */
    void internalFuncEntry ()
    {
    }

private:

    // Private attributes
    //  

    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter::Flight flight;
    RocketData rocketData;
    std::vector<ThreadInterface* > activeThreads;
    std::mutex activeThreadsMutex;
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
    void setRocketData (RocketData new_var)     {
            rocketData = new_var;
    }

    /**
     * Get the value of rocketData
     * @return the value of rocketData
     */
    RocketData getRocketData ()     {
        return rocketData;
    }

    /**
     * Set the value of activeThreads
     * @param new_var the new value of activeThreads
     */
    void setActiveThreads (std::vector<ThreadInterface* > new_var)     {
            activeThreads = new_var;
    }

    /**
     * Get the value of activeThreads
     * @return the value of activeThreads
     */
    std::vector<ThreadInterface* > getActiveThreads ()     {
        return activeThreads;
    }

    /**
     * Set the value of activeThreadsMutex
     * @param new_var the new value of activeThreadsMutex
     */
    void setActiveThreadsMutex (std::mutex new_var)     {
            activeThreadsMutex = new_var;
    }

    /**
     * Get the value of activeThreadsMutex
     * @return the value of activeThreadsMutex
     */
    std::mutex getActiveThreadsMutex ()     {
        return activeThreadsMutex;
    }
private:


    void initAttributes () ;

};

#endif // ROCKETMAINTHREAD_H
