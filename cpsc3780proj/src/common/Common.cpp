#include "Common.h"
#include <iostream>

using namespace std;

char calculateParity(string frameString, bool makeError){
    unsigned int numSet = 0;

    //foreach char in our input string.
    for(char& c : frameString)
    {
        //cout << '\n' << c << ' ' << numSet;
        //see which bits are set and add 1 to numSet for each.
        if((c & 0x01) > 0){ numSet++;};
        if((c & 0x02) > 0){ numSet++;};
        if((c & 0x04) > 0){ numSet++;};
        if((c & 0x08) > 0){ numSet++;};
        if((c & 0x10) > 0){ numSet++;};
        if((c & 0x20) > 0){ numSet++;};
        if((c & 0x40) > 0){ numSet++;};
        if((c & 0x80) > 0){ numSet++;};
    };

if(!makeError){
    //now that we know how many bits are set in the input string
    // we can calculate out parity.
    if(numSet % 2 == 0) return 0x3E;
    else return 0x7E;
}
else{
    
    std::cout<<"\n\n\n\nPARITY ERROR\n\n\n";
    if(numSet % 2 == 0) return 0x7E;
    else return 0x3E;
}
    
};

string generateString(Frame* frame, bool parityError){
    if(frame == nullptr) return "";
    string ret = frame->toString();
    return ret + calculateParity(ret, parityError);
};

void generateFrame(string frameString, Frame** output){

    //it is possible, if not common for the socket to pull in more than frame due to delays in processing.
    // the solution is to simply ignore all but the first one as anything else will get resent at a later date if there is no ACK for it.
    //since this will only affect packets that have a payload this only has to be done for request and data packets (and can be done by only
    //   pulling the specified length from the packet.) 
    //An additional change that must be made is that the parity should be checked after the frame is found and based on frame length.

//    char parity = frameString[frameString.length() -1];
//    string temp = frameString.substr(0, frameString.length() -1);
//
//    char tempParity = calculateParity(temp);
//    std::cout << "\n\nPARITY CHECK: Length: " << frameString.length() << "  R: " << parity << "  C: " << tempParity << "\n\n";
//    if(parity != tempParity){
//        throw FrameParityMissmatchException("The frame's parity bit does not match that calculated for the frame.");
//    };

//temp.erase(0, 2);
//temp.erase(temp.length() - 2, 2);

    //the parity matched so we can generate the frame.
    string temp = frameString;
    char first = temp[0];

    FrameType type = (FrameType)((first & 0xF0) >> 4);
    bool eof = ((first & 0x08) >> 3);
    bool eol = ((first & 0x04) >> 2);
    unsigned int seq = (first & 0x03);
    //remove the type from the string.
    temp.erase(0, 1);
    int packetLength = 1;
    switch(type){
        case FrameType::DATA:
        {
            char length = temp[0];
            temp.erase(0, 1);
            if(length != _DATA_FRAME_EMPTY_LENGTH){

            *output = new DataFrame(seq, eof, eol, temp.substr(0, length));
            packetLength += 1 + length;

            }
            else{
            *output = new DataFrame(seq, eof, eol, "");
            packetLength += 1;
            }

            break;
            //return;
        }
        case FrameType::ACK:
        {
            *output = new  AckFrame(seq);
            break;
            //return;
        }
        case FrameType::NAK:
        {
            *output = new  NakFrame(seq);
            break;
            //return;
        }
        case FrameType::REQUEST:
        {
            char length = temp[0];
            temp.erase(0, 1);
            if(length != _DATA_FRAME_EMPTY_LENGTH){

            *output = new RequestFrame(seq, temp.substr(0, length));
            packetLength += 1 + length;

            }
            else{
            *output = new RequestFrame(seq, "");
            packetLength += 1;
            }
            break;
            //return;
        }
        case FrameType::END:
        {
            *output = new  EndFrame(seq);
            break;
            //return;
        }
        default:
        {
            *output = nullptr;
            throw InvalidFrameException("The frame has an invalid type.\n\nData: " + frameString);
            break;
        }
    }

    char parity = frameString[packetLength];
    //string temp = frameString.substr(0, packetLength - 1);

    char tempParity = calculateParity(frameString.substr(0, packetLength), false);
    std::cout << "\n\nPARITY CHECK: Length: " << frameString.length() << "  R: " << parity << "  C: " << tempParity << "\n\n";
    if(parity != tempParity){
        throw FrameParityMissmatchException("The frame's parity bit does not match that calculated for the frame.");
    };


    //*output = nullptr;

    //if we hit this there was some significant error.
    //throw InvalidFrameException("The frame has an invalid type.");
}