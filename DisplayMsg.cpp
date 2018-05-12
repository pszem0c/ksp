#include "DisplayMsg.h"


DisplayMsg::DisplayMsg(void *_data, int _type) {
    data = _data;
    type = _type;
}

DisplayMsg::~DisplayMsg () { }

void *DisplayMsg::getData()     {
    return data;
}

int DisplayMsg::getType()     {
    return type;
}


