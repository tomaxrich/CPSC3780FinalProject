#ifndef _FRAME_PARITY_MISSMATCH_EXCEPTION_H
#define _FRAME_PARITY_MISSMATCH_EXCEPTION_H

#include <string>

class FrameParityMissmatchException{
private:
    std::string _msg;
public:
    FrameParityMissmatchException(std::string msg) : _msg(msg){};
    ~FrameParityMissmatchException(){};

    std::string description() { return _msg; };
};

#endif