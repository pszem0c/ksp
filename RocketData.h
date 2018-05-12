
#ifndef ROCKETDATA_H
#define ROCKETDATA_H

#include <string>
#include vector


class RocketData
{
public:

    // Constructors/Destructors
    //  


    /**
     * Empty Constructor
     */
    RocketData ();

    /**
     * Empty Destructor
     */
    virtual ~RocketData ();


protected:

public:

protected:

public:

protected:


private:

    // Private attributes
    //  

    int stageCount;
    double availableDeltaV;
    double requestedOrbitAltitude;
public:

private:

public:


    // Private attribute accessor methods
    //  


    /**
     * Set the value of stageCount
     * @param new_var the new value of stageCount
     */
    void setStageCount (int new_var)     {
            stageCount = new_var;
    }

    /**
     * Get the value of stageCount
     * @return the value of stageCount
     */
    int getStageCount ()     {
        return stageCount;
    }

    /**
     * Set the value of availableDeltaV
     * @param new_var the new value of availableDeltaV
     */
    void setAvailableDeltaV (double new_var)     {
            availableDeltaV = new_var;
    }

    /**
     * Get the value of availableDeltaV
     * @return the value of availableDeltaV
     */
    double getAvailableDeltaV ()     {
        return availableDeltaV;
    }

    /**
     * Set the value of requestedOrbitAltitude
     * @param new_var the new value of requestedOrbitAltitude
     */
    void setRequestedOrbitAltitude (double new_var)     {
            requestedOrbitAltitude = new_var;
    }

    /**
     * Get the value of requestedOrbitAltitude
     * @return the value of requestedOrbitAltitude
     */
    double getRequestedOrbitAltitude ()     {
        return requestedOrbitAltitude;
    }
private:


    void initAttributes () ;

};

#endif // ROCKETDATA_H
