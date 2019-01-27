#include "Protocol.h"




void Connection::sendData(Frame* frame){
        std::string gen = generateString(frame, false);
        std::cout << "\n sending data: " << gen << "  << length: " << gen.length() << '\n';
        _dataSocket << gen;
    };void Connection::sendData(Frame* frame, bool error){
        std::string gen = generateString(frame, error);
        std::cout << "\n sending data: " << gen << "  << length: " << gen.length() << '\n';
        _dataSocket << gen;
    };
    void Connection::sendControl(Frame* frame){
        std::string gen = generateString(frame, false);
        std::cout << "\n sending control: " << gen << '\n';
        _controlSocket << gen;
    };

    void Connection::calcNextSeq(){
        _nextSeq = ((_nextSeq + 1) % 3);
    };

    void Connection::startAckTimer(){
        struct itimerval timer;
           // signal(SIGALRM, timer_handler);
        timer.it_value.tv_sec = 2;
        timer.it_value.tv_usec = 250000;
        /* ... and every 250 msec after that. */
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        /* Start a virtual timer. It counts down whenever this process is
        executing. */
        setitimer (ITIMER_REAL, &timer, NULL);
    };

    Connection::Connection(ServerSocket& control, ServerSocket& data){
        _controlSocket = control;
        _dataSocket = data;
        _CONN_ = this;

            //struct itimerval timer;
            signal(SIGALRM, timer_handler);
            // timer.it_value.tv_sec = 2;
            //timer.it_value.tv_usec = 250000;
            /* ... and every 250 msec after that. */
           // timer.it_interval.tv_sec = 2;
            //timer.it_interval.tv_usec = 250000;
            /* Start a virtual timer. It counts down whenever this process is
            executing. */
           // setitimer (ITIMER_REAL, &timer, NULL);
            //_test = 'b';
    };
    void Connection::handleConnection(){
        //we need to wait for either a disconnect, or a request
        //RequestFra* request = nullptr;
        std::cout << "Connection made\n";
        //sleep(1);
        while(!_disconnected){
            handleSendData();
            handleRecieveData();
        };
    };

    void Connection::handleRequest(RequestFrame* frame){
        //_dataSocket << "hello!!";
        std::cout << "\nHandling Request";

        std::cout << "\nOpening file: " << frame->getRequestFile();

        std::cout << "\ntest" << _file.is_open() << "\n";
try{
        _file.open(frame->getRequestFile(), std::ios_base::in);
        if(!_file.is_open()){
            //_test = 'n';
            //the file was not oppened so send a NAK.
            NakFrame response{frame->_seq};
            sendControl((Frame*)&response);
        }
        else{
           // _test = 'o';
            AckFrame response{frame->_seq};
            sendControl((Frame*)&response);
            _sendData = true;
        }
}catch(...){
    std::cout << "\n\n\n\n\n\nERROR WITH FILE OPATION\n\n\n\n";
}

        //sleep(1);
    };
    void Connection::handleEnd(EndFrame* frame){};
    void Connection::handleAck(AckFrame* frame){
        //check that this was for our frame
        if(frame->_seq != _nextSeq) return;
        _lastFrameAck = true;
        if(_lastFrame != nullptr) delete _lastFrame;

        //if _sendData is not set then this was the ack for the last data packet, so mark the connection as closed.
        if(!_sendData) _disconnected = true;
    };
    void Connection::handleNak(NakFrame* frame){
        //check that this was for our frame
        //if(frame->_seq != _nextSeq) return;
        resendLastFrame();
    };
    
    void Connection::handleSendData(){
            sleep(2);
        if(_sendData && _lastFrameAck){
            Frame* sendFrame = nullptr;
            //if(_file.eof()){
            //    //we reached the end of the file so send an eof
            //    sendFrame = (Frame*)(new DataFrame(true, false, "\x04"));
            //}
            //else{
                //out buffer is 1 larger than needed so that it has a null character when turning it to a string.
                char buffer[63];
                std::memset(buffer, 0, 63);
                _file.get(buffer, 62, '\n');
                bool eof = false;
                bool newline = false;

                //I need to check if the next character in the file is the newline character
                if(_file.eof()){
                    //we stopped reading because we hit the end of the file.
                    eof = true;

                    //since we hit the end of the file stop sending data
                    _sendData = false;
std::cout << "\n\n\n\nEOF reached\n";
                }
                else if(_file.peek() == '\n'){
                    newline = true;

                    //remove the character
                    _file.get();
std::cout << "\n\n\n\nEOL reached\n";
                }

                calcNextSeq();
                sendFrame = (Frame*)(new DataFrame(_nextSeq, eof, newline, std::string(buffer)));
                
                _lastFrame = (DataFrame*)sendFrame;
                //_lastFrameTimer = _FRAME_TIMER_LENGTH;
                _lastFrameAck = false;
                startAckTimer();
            //}
bool error = false;
            if(_errorCounter == 5){
                error = true;
                _errorCounter = 0;
            }
            else{
                _errorCounter++;
            }

            sendData(sendFrame, error);

            //Frame* fr2 = new EndFrame();
            //sendData(fr2);
            //delete fr2;
        };
    };
    void Connection::handleRecieveData(){
        try{
            std::cout << "Recieving data\n";
            Frame* recievedFrame = nullptr;

            std::string rawFrame;
            _controlSocket >> rawFrame;
            std::cout << "Recieved:  " << rawFrame << "\n\n";
            //int recieveStatus = socket.recv(rawFrame);
            //if(recieveStatus > 0){
            //We recieved a frame
            generateFrame(rawFrame, &recievedFrame);

            if(recievedFrame != nullptr){
                switch(recievedFrame->_type){
                    case FrameType::REQUEST:{
                        handleRequest((RequestFrame*)recievedFrame);
                        break;
                    }
                    case FrameType::END:{
                        handleEnd((EndFrame*)recievedFrame);
                        break;
                    }
                    case FrameType::ACK:{
                        handleAck((AckFrame*)recievedFrame);
                        break;
                    }
                    case FrameType::NAK:{
                        handleNak((NakFrame*)recievedFrame);
                        break;
                    }
         


               default:{
                        throw InvalidCommunicationException("Recieved a frame that should not be sent to a server");
                        break;
                    }
                };
            };
            //};

                //else we ignore the frame as it wasn't a request.
        } catch(SocketException& e){
            //there was an error, so just assume the connection was lost. 
            std::cout << e.description();
            _disconnected = true;
        } catch(InvalidFrameException& e){
            //there was no frame on the socket.
        };
    };

    void Connection::resendLastFrame(){
        sendData(_lastFrame);
        _lastFrameTimer = _FRAME_TIMER_LENGTH;
        _lastFrameAck = false;
    }

    void Connection::handleTimeout(){
        printf("\n\nTick\n\n");

        if(!_lastFrameAck){
            //if(_lastFrameTimer <= 0){
                resendLastFrame();
                printf("Frame time out\n\n");
            //}
            //else{
            //    _lastFrameTimer--;
            //};
        };
    };



    
Connection* _CONN_ = nullptr;

void timer_handler(int sig_num){
    _CONN_->handleTimeout();

};