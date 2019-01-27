// ClientScenario2.cpp : Defines the entry point for the console application. Client Scenario 2
/// \author Kevin Watchuk
/// \date Nov 20, 2018
///
/// Client for scenario 2
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
			//ClientSocket controlClientSocket(hostchoice, 30000);
			//ClientSocket dataClientSocket(hostchoice, 30001);
			hostchoiceok = true;
		}
		catch (SocketException& e) {
			cout << "Exception was caught:" << e.description() << "\n";
		}
	}


	bool continueRunning = true;
	bool fileExists = false;
	Frame* controlPacketSend = nullptr;
	Frame* controlPacketReceive = nullptr;
	Frame* dataPacketReceive = nullptr;
	string tempRecieve = "";
	bool endOfFile = false;
	ofstream outfile;
	//sleep for a second so that the server has time to get set up.
	//sleep(1);
	unsigned int nextSeq = 0;
	unsigned int lastRecievedSeq = 0;

	try
	{
	while (fileExists == false)
	{
		string choice = "";
		cout << "Welcome to The Client Application" << endl << endl
			<< "Which file would you like to download?" << endl;
		cin >> choice;

		if (choice == "q" || choice == "quit")
		{
			cout << "Quitting Now" << endl;
			continueRunning = false;
			fileExists = true;
		}
		else 
		{

			try
			{
				controlPacketSend = new RequestFrame(nextSeq, choice);
				*controlClientSocket << generateString(controlPacketSend,false); //Request file.
				delete controlPacketSend;
				*controlClientSocket >> tempRecieve; //Ack or nak in response to file request.
				generateFrame(tempRecieve, &controlPacketReceive);  
				
				//TODO have a check here to make sure that the request doesnt timeout
				nextSeq = ((nextSeq + 1) % 4);

				fileExists = controlPacketReceive->_type == FrameType::ACK;
				delete controlPacketReceive;
				if (fileExists == true)
				{
					outfile.open((choice + "d").c_str());
					cout << "Beginning Download" << endl;
				}
				else
				{
					cout << "That file doesn't exist, please try again." << endl;
				}
			}	
			catch (SocketException& e) 
			{
				cout << "Exception was caught:" << e.description() << "\n";
			}


		}
		
		
		}
	}
catch (SocketException& e) {
	cout << "Exception was caught:" << e.description() << "\n";
}

	if (continueRunning == true)
	{
		try 
		{
			while (endOfFile == false)
			{
				*dataClientSocket >> tempRecieve;
				try {
					generateFrame(tempRecieve, &dataPacketReceive);
					if (dataPacketReceive->_seq == lastRecievedSeq) {
						//this was a duplicate
cout<<"\n\nThis was a duplicate\n\n";
						controlPacketSend = new AckFrame(lastRecievedSeq);
						*controlClientSocket << generateString(controlPacketSend, false); //This be that ACK yo!
						delete controlPacketSend;

						continue;//skip the rest of this block.
					}
					lastRecievedSeq = dataPacketReceive->_seq;
					if (dataPacketReceive->_type == FrameType::DATA) {
cout <<"\n\nThis was a data packet\n\n";

cout <<"\n\nSending ACK\n\n";
controlPacketSend = new AckFrame(lastRecievedSeq);
*controlClientSocket << generateString(controlPacketSend, false); //This be that ACK yo!
delete controlPacketSend;

						string data = ((DataFrame*)dataPacketReceive)->getPayload();
						cout << data;
						outfile << data.c_str();
						
						if (dataPacketReceive->_eol == 1)
						{
							cout <<"\n\nTHis was end of line\n\n";
							cout << endl;
							outfile << "\n";
						}
						if (dataPacketReceive->_eof == 1)
						{
							cout <<"\n\nThis was end of file\n\n";
							endOfFile = true;
							outfile.close();
							cout << "File Downloaded." << endl;
							continueRunning = false;
						}
						
//						cout <<"\n\nSending ACK\n\n";
//						controlPacketSend = new AckFrame(lastRecievedSeq);
//						*controlClientSocket << generateString(controlPacketSend, false); //This be that ACK yo!
//						delete controlPacketSend;
					}
					else {
						cout << "This honestly shouldn't have been here yo, I was expecting a data or a end packet, you sent me a weird control packet.";
					}
					//done with the recieved frame.
					delete dataPacketReceive;

				}
				catch (FrameParityMissmatchException& e) {
					cout << "\n\n\n\nrecieved data NAK!\n\n\n";
					controlPacketSend = new NakFrame(0);
					*controlClientSocket << generateString(controlPacketSend, false); //This be that ACK yo!
					delete controlPacketSend;
				}
		catch (InvalidFrameException& e){
cout << "\n\nInvalid Frame: " << e.description() << "\n\n";

		}
				
			}

		}
	
//		catch (SocketException&)
//		{
//			cout << "\n\n\n\nRecieve socket error!\n\n\n";
//		}
		catch (SocketException& e) {
			cout << "Exception was caught:" << e.description() << "\n";
		}
	}
	return 0;
}