#ifndef _INVALID_FRAME_EXCEPTION_H
#define _INVALID_FRAME_EXCEPTION_H

#include <string>

class InvalidFrameException{
private:
    std::string _msg;
public:
    InvalidFrameException(std::string msg) : _msg(msg){};
    ~InvalidFrameException(){};

    std::string description() { return _msg; };
};

#endif