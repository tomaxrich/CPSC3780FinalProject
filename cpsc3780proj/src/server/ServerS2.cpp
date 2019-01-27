#include "common/Frame.h"
#include "common/Common.h"
#include "ServerSocket.h"
#include "common/SocketException.h"
#include "Protocol_s2.h"
#include <pthread.h>

#include <future>

using namespace std;

struct ConnectionPair{
    ServerSocket _control;
    ServerSocket _data;
};

void * thread_start(void *arg){
//    //arg is the ConnectionPair
    ConnectionPair* conn = (ConnectionPair*)arg;
    //pthread_cleanup_push(cleanup_handler, arg);
    Connection c(conn->_control, conn->_data);

//std::mutex _mtx;
//std::mutex _mtx2;

    c.handleConnection();

//hold the thread open for a second so that any timer threads can finish.
    sleep(1);

cout << "\n\nConnection closed\n\n";
    //when we return from handleConnection we can delete our connections.
    delete conn;
cout << "\n\ntest\n\n";
};


//int timerTest(){
//    sleep(20);
//    printf("Timer Tic");
//};

int main(){
    cout << "Starting...\n";
    
    try{
        ServerSocket controlServer(30000);
        ServerSocket dataServer(30001);

        while(true){

      //std::async( std::launch::async, []{ timerTest(); } );
            //ServerSocket controlSocket;
            //ServerSocket dataSocket;
            ConnectionPair* connPair = new ConnectionPair();

            //controlServer.accept(controlSocket);
            controlServer.accept(connPair->_control);
            //dataServer.accept(dataSocket);
            dataServer.accept(connPair->_data);

            cout << "\n\nConnection found\n";

            //try{
            //    while(true){
            //        string data;
            //        cout << "reading data\n";
            //        new_sock >> data;
            //        cout << data << '\n';
            //        cout << "writinging data\n";
            //        new_sock << data;
            //    };
            //} catch(SocketException& e){
            //    cout << "\n\nERROR: could not communicate using the socket!\n";
            //};
            //Connection conn(controlSocket, dataSocket);
            //conn.handleConnection();
            pthread_t thread1;
            int iret1 = pthread_create( &thread1, NULL, thread_start, (void*) connPair);
                if(iret1)
                {
                    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
                    exit(EXIT_FAILURE);
                }

        };
    } catch(SocketException& e){
        cout << "\n\nERROR: " << e.description() << "\n";
    };

    return 0;
};