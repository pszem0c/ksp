#ifndef DISPLAYMSG_H
#define DISPLAYMSG_H


class DisplayMsg {
private:
    void* data;
    int type;

public:    
    DisplayMsg (void* _data, int _type);
    ~DisplayMsg ();

public:
    void* getData ();
    int getType ();

};

#endif // DISPLAYMSG_H
