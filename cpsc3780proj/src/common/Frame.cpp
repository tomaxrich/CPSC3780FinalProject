#include "Frame.h"
#include <cstring>

using namespace std;

Frame::Frame(FrameType type, bool eof, bool eol, unsigned int seq){
    _type = type;
    _eof = eof;
    _eol = eol;
    _seq = seq;
};
Frame::~Frame(){};
string Frame::toString(){
    return string("") + (char)(((unsigned char)_type << 4) | (_eof << 3) | (_eol << 2) | _seq);
};

DataFrame::DataFrame(unsigned int seq, bool eof, bool eol, string data) : Frame(FrameType::DATA, eof, eol, seq){
    //memset(_payload, 0, sizeof(_payload));
    strncpy(_payload, data.c_str(), sizeof(_payload));
    _payloadLength = data.length();
    if(data.length() == 0){
        _payloadLength = _DATA_FRAME_EMPTY_LENGTH;
    };
};
DataFrame::~DataFrame(){};
string DataFrame::toString(){
    if(_payloadLength == _DATA_FRAME_EMPTY_LENGTH){
        return Frame::toString() + (char)_payloadLength;
    };
    return Frame::toString() + (char)_payloadLength + string(_payload, _payloadLength);
};

AckFrame::AckFrame(unsigned int seq) : Frame(FrameType::ACK, false, false, seq){};
AckFrame::~AckFrame(){};

NakFrame::NakFrame(unsigned int seq) : Frame(FrameType::NAK, false, false, seq){};
NakFrame::~NakFrame(){};

RequestFrame::RequestFrame(unsigned int seq, string filename) : Frame(FrameType::REQUEST, false, false, seq){
    //memset(_payload, 0, sizeof(_payload));
    strncpy(_requestFile, filename.c_str(), sizeof(_requestFile));
    _requestFileLength = filename.length();
};
RequestFrame::~RequestFrame(){};
string RequestFrame::toString(){
    //return Frame::toString() + _requestFileLength + string(_requestFile);
    return (Frame::toString() + _requestFileLength) + string(_requestFile,  _requestFileLength);
};

EndFrame::EndFrame(unsigned int seq) : Frame(FrameType::END, false, false, seq){};
EndFrame::~EndFrame(){};