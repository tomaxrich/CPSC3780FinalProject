// ClientScenario1.cpp : Defines the entry point for the console application. Client Scenario 1
/// \author Kevin Watchuk
/// \date Nov 20, 2018
/////
/// Client for scenario 1
#include "ClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <string>
#include "UsefulFunctions.h"
#include <fstream>
#include "common/Frame.h"
#include "common/Common.h"
#include "common/FrameParityMissmatchException.h"


using namespace std;

int main(int argc, int argv[])
{
	string hostchoice = "";
	bool hostchoiceok = false;
	//need to have dummy connections here to make this work.
	ClientSocket* controlClientSocket;// ("localhost", 30000);
	ClientSocket* dataClientSocket;// ("localhost", 30001);
	while (hostchoiceok == false)
	{
		cout << "Please give the hostname of the server:" << endl;
		cin >> hostchoice;
		try {
			controlClientSocket = new ClientSocket(hostchoice, 30000);
			dataClientSocket = new ClientSocket(hostchoice, 30001);
			hostchoiceok = true;
		}
		catch (SocketException& e) {
			cout << "Exception was caught:" << e.description() << "\n";
		}
	}
	bool continueRunning = true;
	bool choiceMade = false;
	while (choiceMade == false)
	{
		char choice = ' ';
		cout << "Welcome to The Client Application" << endl << endl
			<< "Would you like to download file 1? [Y/N]" << endl;
		cin >> choice;

		switch (choice)
		{
		case 'N':
		case 'n':
		{
			cout << "Quitting Now" << endl;
			continueRunning = false;
			choiceMade = true;
			break;
		}
		case 'Y':
		case 'y':
		{
			cout << "Beginning Download now." << endl;
			choiceMade = true;
			break;
		}
		default:
		{
			cout << "Invalid Input" << endl;
			break;
		}
		}
	}

	if (continueRunning == true)
	{
		//sleep for a second so that the server has time to get set up.
		//sleep(1);
		unsigned int nextSeq = 0;
		unsigned int lastRecievedSeq = 0;
		while (continueRunning == true)
		{
			try {
				
				//string controlPacketSend = "";
				Frame* controlPacketSend = nullptr;
				//string controlPacketReceive = "";
				Frame* controlPacketReceive = nullptr;
				//string dataPacketReceive = "";
				Frame* dataPacketReceive = nullptr;
				string tempRecieve = "";
				bool fileExists = false;
				bool endOfFile = false;

				try {
					//controlPacketSend = GeneratePacket('R', "file1");
					controlPacketSend = new RequestFrame(nextSeq, "test.txt");

					//controlClientSocket << controlPacketSend; //Request file.
					*controlClientSocket << generateString(controlPacketSend);
					delete controlPacketSend;
					//controlClientSocket >> controlPacketReceive;  //Ack or nak in response to file request.
					*controlClientSocket >> tempRecieve;
					generateFrame(tempRecieve, &controlPacketReceive);
cout << "\n\n\n\nThe connection is open and a responce to the request has been received!\n\n\n";

					//TODO have a check here to make sure that the request doesnt timeout
					nextSeq = ((nextSeq + 1)%4);

					//fileExists = CheckAckOrNak(controlPacketReceive);
					fileExists = controlPacketReceive->_type == FrameType::ACK;
					delete controlPacketReceive;
					if (fileExists == true)
					{
cout << "\n\n\n\nFile exists!\n\n\n";
						ofstream outfile("file2.txt", fstream::out);

cout << "\n\n\n\nOutfile open!\n\n\n";
						while (endOfFile == false)
						{
cout << "\n\n\n\nwaiting for data!\n\n\n";
							//dataClientSocket >> dataPacketReceive;
							*dataClientSocket >> tempRecieve;

cout << "\n\n\n\nrecieved data!\n" << tempRecieve << "\n\n";
							try{
								generateFrame(tempRecieve, &dataPacketReceive);
								//parity is good.

								if(dataPacketReceive->_seq == lastRecievedSeq){
									//this was a duplicate
									continue;//skip the rest of this block.
cout << "\n\nPacket recieved for second time\n\n";
								}

								lastRecievedSeq = dataPacketReceive->_seq;

								if(dataPacketReceive->_type == FrameType::DATA){
									//string data = dataPacketReceive.substr(4, dataPacketReceive.length());
									//data = BinaryToAscii(data);

									string data = ((DataFrame*)dataPacketReceive)->getPayload();
									cout << data;
							
							
									outfile << data;
									if (dataPacketReceive->_eol == 1)
									{
										cout << endl;
										outfile << string("\n");
									}
									if (dataPacketReceive->_eof == 1)
									{
										endOfFile = true;
										outfile.close();
										//This is for readability.
										//outfile << "\n";
										cout << "\nFile Downloaded." << endl;
										continueRunning = false;
									}
									//controlPacketSend = GeneratePacket('A', "");
									controlPacketSend = new AckFrame(lastRecievedSeq);
									*controlClientSocket << generateString(controlPacketSend); //This be that ACK yo!
									delete controlPacketSend;
								}
								//else if(dataPacketReceive->_type == FrameType::END){
								//	endOfFile = true;
								//	outfile.close();
								//	cout << "File Downloaded." << endl;
								//	continueRunning = false;
								//}
								else{
									cout << "This honestly shouldn't have been here yo, I was expecting a data or a end packet, you sent me a weird control packet.";
								}

								//done with the recieved frame.
								delete dataPacketReceive;

							}catch (FrameParityMissmatchException& e){
cout << "\n\n\n\nrecieved data NAK!\n\n\n";
								controlPacketSend = new NakFrame(0);
								*controlClientSocket << generateString(controlPacketSend); //This be that ACK yo!
								delete controlPacketSend;
							}
							//string type = CheckType(dataPacketReceive);
							//if (type == "Data")
							//if(dataPacketReceive->_type == FrameType::DATA)
							//{
							//	bool parity = CheckParity(dataPacketReceive);
							//	if (parity == true)
							//	{
							//		string data = dataPacketReceive.substr(4, dataPacketReceive.length());
							//		data = BinaryToAscii(data);
							//		cout << data;
							//
							//
							//		outfile << data.c_str();
							//		controlPacketSend = GeneratePacket('A', "");
							//		controlClientSocket << controlPacketSend; //This be that ACK yo!
							//
							//	}
							//	else
							//	{
							//		controlPacketSend = GeneratePacket('N', "");
							//		controlClientSocket << controlPacketSend; //This be that NAK yo!
							//	}
							//}
							//else if (type == "End")
							//{
							//	endOfFile = true;
							//	outfile.close();
							//	cout << "File Downloaded." << endl;
							//	continueRunning = false;
							//}
							//else
							//{
							//	cout << "This honestly shouldn't have been here yo, I was expecting a data or a end packet, you sent me a weird control packet.";
							//}
						}

					}


				}
				catch (SocketException&)
				{
				
cout << "\n\n\n\nRecieve socket error!\n\n\n";exit(-1);
				}
			}
			catch (SocketException& e) {
				cout << "Exception was caught:" << e.description() << "\n";
			}
		}
		

	}
	return 0;
}