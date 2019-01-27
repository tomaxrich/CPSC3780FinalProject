//Useful Functions for messing with the packet
/// \author Kevin Watchuk
/// \date Nov 20, 2018
#include "UsefulFunctions.h"
#include <bitset>
#include <sstream>

std::string DetermineParity(std::string package)
{
	std::string sofar = package;
	int count = 0;
	for (unsigned int i = 0; i < sofar.length(); i++)
	{
		char c = sofar.at(i);
		if (c == '1')
		{
			count++;
		}
	}
	if (count % 2 == 0)
	{
		return "1";
	}
	else
	{
		return "0";
	}
}

std::string AsciiToBinary(std::string data)
{
	std::string binary;
	for (size_t i = 0; i < data.size(); ++i)
	{
		std::bitset<8> bits = (data[i]);
		std::string b = std::string(bits.to_string());
		binary += b;
	}
	return binary;

}

std::string BinaryToAscii(std::string data)
{
	std::string ascii;
	std::stringstream strstream(data);
	while (strstream.good())
	{
		std::bitset<8> bits;
		strstream >> bits;
		char letter = char(bits.to_ulong());
		ascii += letter;
	}
	return ascii;
}

std::string TypetoHeader(char a)
{
	// 5 types, Request, Ack, Nak, Data, End
	switch (a)
	{
	case 'R':
	{
		return "000";
	}
	case 'A':
	{
		return "001";
	}
	case 'N':
	{
		return "010";
	}
	case 'D':
	{
		return "011";
	}
	case 'E':
	{
		return "100";
	}

	default:
	{
		return "this shouldn't have happened...";
	}
	}

}
bool CheckParity(std::string packet)
{
	int count = 0;
	if (packet.length() <= 4)
	{
		return true;
	}
	else
	{
		std::string parityBit = packet.substr(3, 1);

		std::string data = packet.substr(4, packet.length());
		for (unsigned int i = 0; i < data.length(); i++)
		{
			char c = data.at(i);
			if (c == '1')
			{
				count++;
			}
		}
		if (count % 2 == 0 && parityBit == "1" || count % 2 != 0 && parityBit == "0")
		{

			return true;
		}
		else
		{

			return false;
		}

	}


}

bool CheckAckOrNak(std::string packet)
{
	std::string decide = packet.substr(0, 3);

	if (decide == "001")
	{
		return true;
	}
	else
	{
		return false;
	}

}

std::string CheckType(std::string packet)
{
	std::string type = packet.substr(0, 3);
	if (type == "000")
	{
		return "Request";
	}
	else if (type == "001")
	{
		return "Ack";
	}
	else if (type == "010")
	{
		return "Nak";
	}
	else if (type == "011")
	{
		return "Data";
	}
	else if (type == "100")
	{
		return "End";
	}
	else
	{
		return "error";
	}
}


std::string GeneratePacket(char type, std::string data)
{
	std::string header;
	std::string package;
	if (data != "")
	{
		package = AsciiToBinary(data);
	}
	else
	{
		package = "";
	}
	std::string typeString = TypetoHeader(type);
	std::string parity = DetermineParity(package);
	header = typeString + parity;
	return header + package;

}