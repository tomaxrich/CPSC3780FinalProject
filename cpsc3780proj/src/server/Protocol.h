#include "common/Frame.h"
#include "common/Common.h"
#include "common/InvalidCommunicationException.h"
#include "ServerSocket.h"
#include "common/SocketException.h"

#include <string>
#include <fstream>
#include <sys/time.h>
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <thread>

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define _FRAME_TIMER_LENGTH 2

class Connection;

extern Connection* _CONN_;// = nullptr;

void timer_handler(int sig_num);

class Connection{
private:
    bool _sendData = false;
    bool _disconnected = false;
    std::ifstream _file;
    //char _buffer[_DATA_FRAME_MAX_DATA];
    ServerSocket _controlSocket;
    ServerSocket _dataSocket;
    DataFrame* _lastFrame = nullptr;
    int _lastFrameTimer;
    bool _lastFrameAck = true;
    unsigned int _nextSeq = 0;
    unsigned int _errorCounter = 0;

    void sendData(Frame* frame);
    void sendData(Frame* frame, bool parityError);
    void sendControl(Frame* frame);

    void calcNextSeq();

    void startAckTimer();

public:
    Connection(ServerSocket& control, ServerSocket& data);
    void handleConnection();
    void handleRequest(RequestFrame* frame);
    void handleEnd(EndFrame* frame);
    void handleAck(AckFrame* frame);
    void handleNak(NakFrame* frame);
    
    void handleSendData();
    void handleRecieveData();

    void resendLastFrame();

    void handleTimeout();


    friend void timer_handler(int sig_num);
};


#endif