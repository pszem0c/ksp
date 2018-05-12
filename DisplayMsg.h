
#ifndef DISPLAYMSG_H
#define DISPLAYMSG_H

#include <string>
#include vector


class DisplayMsg
{
public:

    // Constructors/Destructors
    //  


    /**
     * Empty Constructor
     */
    DisplayMsg ();

    /**
     * Empty Destructor
     */
    virtual ~DisplayMsg ();



    /**
     * @param  _data
     * @param  _type
     */
     DisplayMsg (void* _data, int _type)
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

    void* data;
    int type;
public:

private:

public:


    // Private attribute accessor methods
    //  


    /**
     * Set the value of data
     * @param new_var the new value of data
     */
    void setData (void* new_var)     {
            data = new_var;
    }

    /**
     * Get the value of data
     * @return the value of data
     */
    void* getData ()     {
        return data;
    }

    /**
     * Set the value of type
     * @param new_var the new value of type
     */
    void setType (int new_var)     {
            type = new_var;
    }

    /**
     * Get the value of type
     * @return the value of type
     */
    int getType ()     {
        return type;
    }
private:


    void initAttributes () ;

};

#endif // DISPLAYMSG_H
