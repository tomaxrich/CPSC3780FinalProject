#include "ClientSocket.h"
#include "common/SocketException.h"
#include "common/Common.h"
#include <iostream>
#include <string>


int main(int argc, int argv[])
{
   try{
       // Replace "localhost" with the hostname
      // that you're running your server.
      ClientSocket client_socket("localhost", 30000);
      ClientSocket client_socket2("localhost", 30001);
      std::string reply;
      // Usually in real applications, the following
      // will be put into a loop. 
      try {
        RequestFrame fr("test.txt");
        std::cout << "\n\nIntial frame:    " << fr.toString() << '\n';
        std:: cout << "\n\nAfter Gen:      " << generateString(&fr) << '\n';
	    client_socket << generateString(&fr);
        client_socket >> reply;
        Frame* frame;
        generateFrame(reply, &frame);

        if(frame->_type == FrameType::ACK) {std::cout << "\n ACK!\n";};
        if(frame->_type == FrameType::NAK){std::cout << "\n NAK....\n";};

        while(true){
            std::cout << "Recieved:   " << reply << "\n\n";
            client_socket2 >> reply;
        }

	    //client_socket >> reply;
      }
      catch(SocketException&){
      }
      std::cout << "We received this response from the server:\n\"" << reply << "\"\n";;
   }
   catch(SocketException& e){
      std::cout << "Exception was caught:" << e.description() << "\n";
   }
    
   return 0;
};