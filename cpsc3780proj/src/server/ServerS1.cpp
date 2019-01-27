#include "common/Frame.h"
#include "common/Common.h"
#include "ServerSocket.h"
#include "common/SocketException.h"
#include "Protocol.h"

using namespace std;

int main(){
    cout << "Starting...\n";
    
    try{
        ServerSocket controlServer(30000);
        ServerSocket dataServer(30001);

        while(true){
            ServerSocket controlSocket;
            ServerSocket dataSocket;

            controlServer.accept(controlSocket);
            dataServer.accept(dataSocket);

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
            Connection conn(controlSocket, dataSocket);
            conn.handleConnection();
        };
    } catch(SocketException& e){
        cout << "\n\nERROR: " << e.description() << "\n";
    };

    return 0;
};