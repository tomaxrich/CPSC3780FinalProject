#include "common/Frame.h"
#include "common/Common.h"
#include "common/InvalidCommunicationException.h"
#include <string>
#include <fstream>
#include <sys/time.h>
#include <signal.h>
#include <cstring>
#include <unistd.h>
#include <thread>

#include <future>
#include <mutex>

#include "ServerSocket.h"
#include "common/SocketException.h"

#define _FRAME_TIMER_LENGTH 2

class Connection;

extern Connection* _CONN_;// = nullptr;
//extern std::mutex _mtx;
//extern std::mutex _mtx2;

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
    unsigned long _ackCount = 0;
    std::future<void> _currentAckTimer;
    unsigned int _errorCounter = 0;

    std::mutex _mtx;
std::mutex _mtx2;

    void sendData(Frame* frame);
    void sendData(Frame* frame, bool parityError);
    void sendControl(Frame* frame);

    void calcNextSeq();

    void timer(unsigned long count);

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

    void handleTimeout(int count);

    friend void timer_handler(int sig_num);
};
