#ifndef _FRAME_H
#define _FRAME_H

#include <string>

#define _MAX_FRAME_SIZE 64


//These are the types of frames, chosen to have values that would reuire multiple bit errors to overlap.
//enum class FrameType : unsigned char{
//    DATA = 0x0F,    //00001111
//    ACK = 0xFF,     //11111111
//    NAK = 0xAA,     //10101010
//    REQUEST = 0x66, //01010101
//    END = 0xF0      //11110000
//};
enum class FrameType : unsigned char{
    DATA = 0x01,
    ACK = 0x02,
    NAK = 0x03,
    REQUEST = 0x04,
    END = 0x05
};

struct Frame{
    FrameType _type : 4;
    bool _eof : 1;
    bool _eol : 1;
    unsigned int _seq : 2;

    Frame(FrameType, bool, bool, unsigned int);
    virtual ~Frame();

    virtual std::string toString();
};

#define _DATA_FRAME_MAX_DATA _MAX_FRAME_SIZE - 1 - 1
#define _DATA_FRAME_EMPTY_LENGTH 95

struct DataFrame : public Frame{
    unsigned char _payloadLength = _DATA_FRAME_EMPTY_LENGTH;
    char _payload[_DATA_FRAME_MAX_DATA];

    std::string getPayload(){
        if(_payloadLength == _DATA_FRAME_EMPTY_LENGTH){
            return "";
        }
        return std::string(_payload, _payloadLength);
    };

    DataFrame(unsigned int, bool, bool, std::string data);
    virtual ~DataFrame();

    std::string toString();
};

struct AckFrame : public Frame{
    AckFrame(unsigned int);
    virtual ~AckFrame();

    //nothing new so we don't have to override this.
    //string toString();
};

struct NakFrame : public Frame{
    NakFrame(unsigned int);
    virtual ~NakFrame();

    //nothing new so we don't have to override this.
    //string toString();
};

struct RequestFrame : public Frame{
    char _requestFileLength;
    char _requestFile[_MAX_FRAME_SIZE - sizeof(Frame) - sizeof(_requestFileLength)];

    std::string getRequestFile(){return std::string(_requestFile, _requestFileLength);};

    RequestFrame(unsigned int, std::string filename);
    virtual ~RequestFrame();

    std::string toString();
};

struct EndFrame : public Frame{
    EndFrame(unsigned int);
    virtual ~EndFrame();

    //nothing new so we don't have to override this.
    //string toString();
};

#endif