#include "Protocol_s2.h"

Connection* _CONN_ = nullptr;
//std::mutex _mtx;
//std::mutex _mtx2;


void Connection::sendData(Frame* frame){
        std::string gen = generateString(frame, false);
        std::cout << "\n sending data: " << gen << "  << length: " << gen.length() << '\n';
        _dataSocket << gen;
    };
    void Connection::sendData(Frame* frame, bool parityError){
        std::string gen = generateString(frame, parityError);
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

    void Connection::timer(unsigned long count){
        //usleep(5000000);
        //_mtx.lock();
        usleep(300000);
        _mtx.lock();
       // _mtx2.lock();
        if(_ackCount != count){
        _mtx.unlock();
            std::cout << "\n\ntimer expired but is not needed.\n\n";
        }
        else{
        _mtx.unlock();
            std::cout <<"\n\ntimer expired\n\n";
        /*if(_ackCount == count)*/ handleTimeout(count);
        //_mtx.unlock();
        }
        //_mtx2.unlock();
        //_mtx.unlock();
    };

    void Connection::startAckTimer(){
        //struct itimerval timer;
        //   // signal(SIGALRM, timer_handler);
        //timer.it_value.tv_sec = 2;
        //timer.it_value.tv_usec = 250000;
        ///* ... and every 250 msec after that. */
        //timer.it_interval.tv_sec = 0;
        //timer.it_interval.tv_usec = 0;
        ///* Start a virtual timer. It counts down whenever this process is
        //executing. */
        //setitimer (ITIMER_REAL, &timer, NULL);
        //_mtx2.lock();
        _currentAckTimer = std::async( std::launch::async, [this]{ timer(_ackCount); } );
        //_mtx2.unlock();
    }


    Connection::Connection(ServerSocket& control, ServerSocket& data){
        _controlSocket = control;
        _dataSocket = data;
        //_CONN_ = this;

            //struct itimerval timer;
            //signal(SIGALRM, timer_handler);
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
            try{
            //_mtx.lock();
            handleSendData();
            //_mtx.unlock();
            handleRecieveData();
            }
            catch(...){
                _disconnected = true;
            }
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
        //_mtx.lock();
        //check that this was for our frame
//        if(frame->_seq != _nextSeq) return;
std::cout <<"\n\nACK RECIEVED\n\n";
        _lastFrameAck = true;
//_mtx2.lock();
        _ackCount++;
//_mtx2.unlock();

//_mtx.lock();

        if(_lastFrame != nullptr) delete _lastFrame;
        _lastFrame = nullptr;
//_mtx.unlock();

        //if _sendData is not set then this was the ack for the last data packet, so mark the connection as closed.
        if(!_sendData) _disconnected = true;
        //_mtx.unlock();
    };
    void Connection::handleNak(NakFrame* frame){
        //check that this was for our frame
        //if(frame->_seq != _nextSeq) return;
        //resendLastFrame();

        //need to do this to prevent some weird things
        _ackCount++;


        bool error = false;
            if(_errorCounter == 5){
                error = true;
                _errorCounter = 0;
            }
            else{
                _errorCounter++;
            }

            sendData(_lastFrame, error);
std::cout << "\n\nData sent\n\n";
            startAckTimer();

    };
    
    void Connection::handleSendData(){
//            sleep(2);
        if(_sendData && _lastFrameAck){

//wait for the ack timer to stop.
//_currentAckTimer.get();

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

                //if(_file.peek() == '\n'){
                //    _file.get();
                //}

                //I need to check if the next character in the file is the newline character
//                if(_file.eof()){
                if(!_file.good()){
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

//_mtx.lock();

                calcNextSeq();
                sendFrame = (Frame*)(new DataFrame(_nextSeq, eof, newline, std::string(buffer)));

                _lastFrame = (DataFrame*)sendFrame;



                //_lastFrameTimer = _FRAME_TIMER_LENGTH;
                _lastFrameAck = false;
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
std::cout << "\n\nData sent\n\n";
                startAckTimer();
//_mtx.unlock();
std::cout << "\n\nAck timer started\n\n";

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
            std::cout << "\n\nRecieved:  " << rawFrame << "\n\n";
        _mtx.lock();
            std::cout <<"\n\nMUTEXT LOCK ESTABLISHED\n\n";
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

        _mtx.unlock();
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
        //_mtx.lock();
        sendData(_lastFrame);
        //_lastFrameTimer = _FRAME_TIMER_LENGTH;
        //_lastFrameAck = false;
std::cout << "\n\n Data Resend\n\n";
        //startAckTimer();
        //_mtx.unlock();

    };

    void Connection::handleTimeout(int count){
        bool lastAck = false;
        int lastCount = 0;

        _mtx.lock();
        lastAck = _lastFrameAck;
        lastCount = _ackCount;
        _mtx.unlock();

        while(!lastAck && lastCount == count && _lastFrame != nullptr){
            printf("\n\nTick needed\n\n");
            //if(_lastFrame != nullptr){
                resendLastFrame();

            //we resent the frame now we just have to wait.
            
            usleep(300000);
                printf("\n\n\nLAST FRAME NOT NULL\n\n\n");

        _mtx.lock();
        lastAck = _lastFrameAck;
        lastCount = _ackCount;
        _mtx.unlock();
            //};
        };
        //else{
        //    printf("\n\nTick NOT needed\n\n");
       // }

  //      if(!_lastFrameAck){
  //          //if(_lastFrameTimer <= 0){
  //              resendLastFrame();
  //              printf("Frame time out\n\n");
  //          //}
  //          //else{
  //          //    _lastFrameTimer--;
  //          //};
  //      };
    };