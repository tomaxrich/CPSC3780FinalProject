#include "common/Frame.h"
#include "common/Common.h"
#include "ServerSocket.h"
#include "common/SocketException.h"
#include "Protocol.h"

using namespace std;

int main() {
	cout << "Starting...\n";

	try {
		ServerSocket controlServer(30000);
		ServerSocket dataServer(30001);

		while (true) {


			Frame* controlPacketReceive = nullptr;
			Frame* controlPacketSend = nullptr;
			Frame* dataPacketSend = nullptr;
			string tempRecieve = "";

			controlServer.accept(controlSocket);
			dataServer.accept(dataSocket);

			ifstream inputFile;
			bool inputfileok = false;
			bool continuerunning = true;
			bool downloadcomplete = false;

			while (continuerunning == true)
			{
				while (inputfileok == false)
				{
					controlServer >> tempRecieve;
					generateFrame(tempRecieve, &controlPacketReceive);
					inputFile.open(((DataFrame*)controlPacketReceive)->getPayload());
					inputfileok = inputFile.good();
					delete controlPacketReceive;

					if (inputfileok == true)
					{
						controlPacketSend = new AckFrame();
						controlServer << generateString(controlPacketSend);
					}
					else
					{
						controlPacketSend = new NakFrame();
						controlServer << generateString(controlPacketSend);
					}

				}
				string payload = "";
				while (!inputFile.eof)
				{
					if (payload.length() == 0)
					{
						getline(inputFile, payload);
					}
					if (payload.length() >= 64)
					{
						boolean resend = true;
						dataPacketSend = new DataFrame(0, 0, payload.substr(0, 63));
						while (resend == true)
						{
							dataServer << generateString(dataPacketSend);

							controlServer >> tempRecieve;
							generateFrame(tempRecieve, &controlPacketReceive);
							if (controlPacketReceive->_type == FrameType::ACK)
							{
								delete dataPacketSend;
								resend = false;
								payload = payload.substr(64, payload.length());
							}
							delete controlPacketReceive;
						}
					}
					else
					{
						boolean resend = true;
						if (inputFile.eof)
						{
							dataPacketSend = new DataFrame(1, 1, payload);
						}
						else
						{
							dataPacketSend = new DataFrame(0, 1, payload);
						}

						while (resend == true)
						{
							dataServer << generateString(dataPacketSend);

							controlServer >> tempRecieve;
							generateFrame(tempRecieve, &controlPacketReceive);
							if (controlPacketReceive->_type == FrameType::ACK)
							{
								delete dataPacketSend;
								resend = false;
								payload = "";
							}
							delete controlPacketReceive;
						}

					}
				}

			}
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
	}
	catch (SocketException& e) {
		cout << "\n\nERROR: " << e.description() << "\n";
	};

	return 0;
};