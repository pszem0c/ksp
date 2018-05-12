
#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H
#include "ThreadInterface.h"

#include <string>
#include vector


class DisplayThread : virtual public ThredInterface
{
public:

    // Constructors/Destructors
    //  


    /**
     * Empty Constructor
     */
    DisplayThread ();

    /**
     * Empty Destructor
     */
    virtual ~DisplayThread ();



    /**
     * @param  data
     * @param  type
     */
    void sendMsg (int data, int type)
    {
    }


    /**
     * @param  data
     * @param  type
     */
    void sendMsg (double data, int type)
    {
    }

protected:

public:

protected:

public:

protected:



    /**
     */
    void internalThreadEntry ()
    {
    }

private:

    // Private attributes
    //  

    std::deque<DisplayMsg* > displayMsgQueue;
    std::mutex displayMsgQueueMutex;
public:

private:

public:


    // Private attribute accessor methods
    //  


    /**
     * Set the value of displayMsgQueue
     * @param new_var the new value of displayMsgQueue
     */
    void setDisplayMsgQueue (std::deque<DisplayMsg* > new_var)     {
            displayMsgQueue = new_var;
    }

    /**
     * Get the value of displayMsgQueue
     * @return the value of displayMsgQueue
     */
    std::deque<DisplayMsg* > getDisplayMsgQueue ()     {
        return displayMsgQueue;
    }

    /**
     * Set the value of displayMsgQueueMutex
     * @param new_var the new value of displayMsgQueueMutex
     */
    void setDisplayMsgQueueMutex (std::mutex new_var)     {
            displayMsgQueueMutex = new_var;
    }

    /**
     * Get the value of displayMsgQueueMutex
     * @return the value of displayMsgQueueMutex
     */
    std::mutex getDisplayMsgQueueMutex ()     {
        return displayMsgQueueMutex;
    }
private:


    void initAttributes () ;

};

#endif // DISPLAYTHREAD_H
