#ifndef _COMMON_H
#define _COMMON_H

#include "Frame.h"

#include "FrameParityMissmatchException.h"
#include "InvalidFrame.h"

//the returned parity is padded with 1s so that it isn't a null char.
char calculateParity(std::string frameString);
std::string generateString(Frame* frame, bool parityError);

void generateFrame(std::string frameString, Frame** output);

#endif